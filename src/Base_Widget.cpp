/*
* Copyright(c) 2017 dragon jiang<jianlinlong@gmail.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "Base_Widget.h"
#include "Title_Bar.h"
#include "Skin.h"
#include "NcFramelessHelper.h"
#include <QSettings>
#include <QDebug>

Base_Widget_Imp::Base_Widget_Imp(QWidget *owner, Skin *skin, bool is_dialog)
  :  q(owner)
    ,resize_temp_disabled_(false)
    ,resizable_(true)
    ,skin_(skin)
    ,recreate_bg_pixmap_(true)
    ,bg_pixmap_(NULL)
    ,child_bkg_alpha_(0)
{
    init(is_dialog);
}

Base_Widget_Imp::~Base_Widget_Imp()
{
    //��󻯺�Ͳ������С�ˣ�����ָ��󴰿�̫��
    if (!isMaximized())
    {
        save_geometry(save_object_name_);
    }

    qDeleteAll(alpha_cache_);
    alpha_cache_.clear();

    if (bg_pixmap_)
    {
        delete bg_pixmap_;
        bg_pixmap_ = NULL;
    }

    delete resize_helper_;
}

int Base_Widget_Imp::init(bool is_dialog)
{
    if (NULL == skin_)
    {
        skin_ = skinman->default_skin();
    }

    //ͨ���߿�������С
    resize_helper_ = new NcFramelessHelper();
    resize_helper_->activateOn(q);

    layout_ = new QVBoxLayout();
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);

    title_bar_ = new Title_Bar(q, skin_);
    title_bar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout_->addWidget(title_bar_, 0, Qt::AlignTop);
    q->setLayout(layout_);

    q->setWindowFlags( q->windowFlags() | Qt::FramelessWindowHint); 
    //setAttribute(Qt::WA_OpaquePaintEvent);
#ifndef APS     //��ЩXP��ʾ����������ĳ̨APS�ϵ�XP���ԣ�����Ϊ�����һ�д��뵼��
    q->setAttribute(Qt::WA_TranslucentBackground);
#endif

    if (is_dialog)
    {
        q->setWindowFlags(q->windowFlags() | Qt::Dialog);
    }

    load_skin_internal();
    if (-1 == skin_->titlebar_showstyle())  //����ָ����ʾ��Щ�˵���ť
    {
        int s = is_dialog ? Title_Bar::DIALOG_STYLE : Title_Bar::MAIN_FORM_SKIN_STYLE;
        title_bar_->set_show_style(s);
    }

    return 0;
}

void Base_Widget_Imp::resizeEvent( QResizeEvent * e)
{
    if (e->size() == e->oldSize()) 
    { 
        return; 
    }

    recreate_bg_pixmap_ = true;

    //�����
    if (isMaximized())
    {
        //��Բ��,����ֹ�ı䴰�ڴ�С
        q->clearMask();
        if (resizable_)
        {
            resize_temp_disabled_ = true;
            resize_helper_->setWidgetResizable(false);
            resize_helper_->setWidgetMovable(false);
        }

        //��󻯺��ޱ߿��ޱ߾�
        layout_->setContentsMargins(0, 0, 0, 0);
        return;

    }else{

        //�ָ����ڵı߿�߾�
        if (layout_->contentsMargins().isNull())
        {
            layout_->setContentsMargins(skin_->background().margin());
        }

        //��󻯺󻹸�: ����ı䴰�ڴ�С
        if (resizable_ && resize_temp_disabled_)
        {
            resize_temp_disabled_ = false;
            resize_helper_->setWidgetResizable(true);
            resize_helper_->setWidgetMovable(true);
        }
    }

#if 1
    //Բ�Ǵ���
    QBitmap  pixmap(e->size());  //����һ��λͼ
    QPainter painter(&pixmap);  //QPainter������λͼ�ϻ滭
    // Բ��ƽ��
    painter.setRenderHints(QPainter::Antialiasing, true); 

    //���λͼ���ο�(�ð�ɫ���)
    QRect r = q->rect();
    painter.fillRect(r, Qt::color0);
    painter.setBrush(Qt::color1);
    //��λͼ�ϻ�Բ�Ǿ���(�ú�ɫ���)
    painter.drawRoundedRect(r, skin_->round_radius_x(), skin_->round_radius_y());
    painter.end();

    //ʹ��setmask���˼���
    q->setMask(pixmap);
#endif
}

void Base_Widget_Imp::paintEvent( QPaintEvent *event)
{
    //qDebug() << "Base_Widget_Imp::paintEvent()";
    if (recreate_bg_pixmap_ || !bg_pixmap_)
    {
        recreate_bg_pixmap_ = false;
        qDeleteAll(alpha_cache_);
        alpha_cache_.clear();

        QRect r = q->rect();  //background_rect()

        delete bg_pixmap_; //it's safe to delete null
        bg_pixmap_ = new QPixmap(r.width(), r.height());
        bg_pixmap_->fill(Qt::transparent);//Qt::black

        QPainter painter(bg_pixmap_);
        painter.setRenderHint(QPainter::Antialiasing, true);

        //�߿򱳾�ͼ
        const Border_Image &bi = skin_->background();
        const QMargins     &m  = bi.border();
        if (isMaximized())  //��󻯺��ޱ߿�
        {
            //����ֻ��һ��borderimage��Ϊ��������������������󻯺����Ҫ��borderimageȥ���߿����Ϊ����ͼ
            //��rect�Ŵ�����ʹ�߿򿴲���. 
            r.adjust(-m.left(), -m.top(), m.right(), m.bottom()); 
        }
        qDrawBorderPixmap(&painter, r, m, bi.pixmap());

        //�ͻ���ͼ
        const QPixmap& bmp = skin_->client_pixmap();
        if (!bmp.isNull())
        {
            r = client_rect();

            QPixmap pixmap(r.size());
            {
                QPainter p(&pixmap);
                //1-�����Ϲ̶����������죻2-���Ϲ̶�����������
                switch (skin_->client_draw_type())
                {
                case 1:
                    draw_bg_topleft(&p, pixmap.rect(), bmp);
                    break;
                case 2:
                    draw_bg_topright(&p, pixmap.rect(), bmp);
                    break;
                default:
                    p.drawPixmap(pixmap.rect(), bmp);
                    break;
                }
            }

            painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);//CompositionMode_DestinationAtop,CompositionMode_SoftLight,CompositionMode_Multiply
            painter.drawPixmap(r.left(), r.top(), pixmap);
        }
    }

    QPainter painter(q);
    painter.drawPixmap(0, 0, *bg_pixmap_);
}

void Base_Widget_Imp::draw_bg_topleft( QPainter *painter, const QRect& r, const QPixmap& img )
{
    //ͼƬ��Ҫ��������󣬼���
    int w = qMin(img.width(), r.width());
    int h = qMin(img.height(), r.height());

    painter->drawPixmap(r.left(), r.top(), img, 0, 0, w, h); 

    //ͼƬ��ȱ�Ҫ��������С��������
    if (w < r.width())
    {
        QRect dst(r.left() + w, r.top(), r.width() - w, h/*r.height()*/);
        QRect src(w - 1, 0, 1, h);
        painter->drawPixmap(dst, img, src);
    }

    //ͼƬ�߶ȱ�Ҫ��������С������߶�
    if (h < r.height())
    {
        QRect dst(r.left(), r.top() + h, w/*r.width()*/, r.height() - h);
        QRect src(0, h - 1, w, 1);
        painter->drawPixmap(dst, img, src);
    }

    //ͼƬ�߶ȡ���ȶ���Ҫ��������С�����������½�
    if (w < r.width() && h < r.height())
    {
        QRect dst(r.left() + w, r.top() + h, r.width() - w, r.height() - h);
        QRect src(w - 1, h - 1, 1, 1);  //�����һ��������
        painter->drawPixmap(dst, img, src);
    }
}

void Base_Widget_Imp::draw_bg_topright( QPainter *painter, const QRect& r, const QPixmap& img )
{
    int w = qMin(img.width(), r.width());
    int h = qMin(r.height(), img.height());

    //����, ͼ�������������
    if (r.width() > img.width())
    {
        QRect src(0, 0, 1, h);

        QRect dst(r.left(), r.top(), r.width() - img.width(), h);
        painter->drawPixmap(dst, img, src);
    }

    //����()
    {
        QRect src(img.width() - w, 0, w, h);
        QRect dst(r.left() + r.width() - w, r.top(), w, h);
        painter->drawPixmap(dst, img, src);
    }

    //����(����)
    if (r.width() > img.width() && r.height() > img.height())
    {
        QRect src(0, h - 1, 1, 1);
        QRect dst(0, h, r.width() - img.width(), r.height() - img.height());
        painter->drawPixmap(dst, img, src);
    }

    //����(����), ͼ���±���������
    if (r.height() > img.height())
    {
        QRect src(img.width() - w, h - 1, w, 1);
        QRect dst(r.left() + r.width() - w, h, w, r.height() - img.height());
        painter->drawPixmap(dst, img, src);
    }
}

bool Base_Widget_Imp::isMaximized() const
{
    return title_bar_->is_window_maximized();
}

QString Base_Widget_Imp::windowTitle() const
{
    return title_bar_->text();
}

void Base_Widget_Imp::setWindowTitle( const QString & title)
{
    title_bar_->set_text(title);
}

void Base_Widget_Imp::set_resizable( bool b )
{
    if (b == resizable_)
    {
        return;
    }

    if (resizable_)
    {
        resize_helper_->removeFrom(q);
    }else{
        resize_helper_->activateOn(q);
    }
    resizable_ = b;
}

void Base_Widget_Imp::load_skin( Skin* skin/*=NULL*/ )
{
    if (NULL != skin)
    {
        skin_ = skin;
    }

    title_bar_->load_skin(skin_);
    load_skin_internal();
}

void Base_Widget_Imp::load_skin_internal()
{
    layout_->setContentsMargins(skin_->background().margin());
    q->setStyleSheet(skin_->widget_qss());
}

int Base_Widget_Imp::load_geometry(const QString& name)
{
    if (name.isEmpty())
    {
        return -1;
    }
    save_object_name_ = name;

    QString file_name = qApp->applicationDirPath() + "/gemoetry.dat";
    QSettings ini(file_name, QSettings::IniFormat);
    ini.setIniCodec("GB2312");     //GB2312-->ini���뱣��Ϊascii��, ����������

    ini.beginGroup(name);
    int w = ini.value("width", 0).toInt();
    int h = ini.value("height", 0).toInt();
    ini.endGroup();

    if (w > 0 && h > 0)
    {
        q->resize(w, h);
        return 0;
    }

    return -2;
}

int Base_Widget_Imp::save_geometry(const QString& name)
{
    if (name.isEmpty())
    {
        return -1;
    }

    QString file_name = qApp->applicationDirPath() + "/gemoetry.dat";
    QSettings ini(file_name, QSettings::IniFormat);
    ini.setIniCodec("GB2312");     //GB2312-->ini���뱣��Ϊascii��, ����������

    ini.beginGroup(name);
    ini.setValue("width",  q->width());
    ini.setValue("height", q->height());
    ini.endGroup();

    return 0;
}

int Base_Widget_Imp::save_geometry()
{
    return save_geometry(save_object_name_);
}

void Base_Widget_Imp::move_center()
{
    QWidget *p = (QWidget*)q->parent();
    if (NULL != p)
    {
        CenterParent(p->window(), q);
    }else{
        CenterParent(QApplication::desktop(), q);
    }
}

void Base_Widget_Imp::setup_widget( QWidget *content )
{
    title_bar_->set_text(content->windowTitle());
    layout_->addWidget(content, 1);

    int ret = load_geometry(content->objectName());
    if (0 != ret)
    {
        QMargins margin = skin_->background().margin();
        QSize size = content->size();
        int h = size.height() + margin.top()  + margin.bottom() + title_bar_->height();
        int w = size.width()  + margin.left() + margin.right();
        q->resize(w, h);
    }

    //�����ھ���
    move_center();
}

QRect Base_Widget_Imp::client_rect() const
{
    QRect r(q->rect());
    if (!isMaximized())
    {
        const QMargins& m = skin_->background().margin();
        r.adjust(m.left(), m.top(), -m.right(), -m.bottom());
    }

    return r;
}

const QPixmap* Base_Widget_Imp::bg_pixmap() const
{
    return bg_pixmap_;
}

QPoint Base_Widget_Imp::window_to_client( const QPoint& pt )
{
    QPoint ret(pt);
    if (!isMaximized())
    {
        const QMargins& m = skin_->background().margin();
        ret.rx() -= m.left();
        ret.ry() -= m.top();
    }
    return ret;
}

QRect Base_Widget_Imp::window_to_client( const QRect& rt )
{
    QRect ret(rt);
    ret.moveTopLeft(window_to_client(rt.topLeft()));
    return ret;
}

QPoint Base_Widget_Imp::client_to_window( const QPoint& pt )
{
    QPoint ret(pt);
    if (!isMaximized())
    {
        const QMargins& m = skin_->background().margin();
        ret.rx() += m.left();
        ret.ry() += m.top();
    }
    return ret;
}

QRect Base_Widget_Imp::client_to_window( const QRect& rt )
{
    QRect ret(rt);
    ret.moveTopLeft(client_to_window(rt.topLeft()));
    return ret;
}

bool Base_Widget_Imp::eventFilter( QObject *o, QEvent *e )
{
    //qDebug() << o;
    if (e->type() == QEvent::Paint && bg_pixmap_)
    {
        QWidget     *w  = (QWidget*)o;
        QPixmap *pixmap = alpha_cache_.value(w, NULL);
        if (!pixmap)
        {
            QRect r(w->rect());
            r.moveTopLeft( w->mapTo(q, r.topLeft()) );

            pixmap = new QPixmap(w->rect().size());
            pixmap->fill(Qt::transparent);
            {
                QPainter p(pixmap);
                p.setCompositionMode(QPainter::CompositionMode_Source);
                p.drawPixmap(QPoint(0, 0), *bg_pixmap_, r);  //�������ڵı���ͼ(1)

                p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                p.fillRect(pixmap->rect(), QColor(0, 0, 0, child_bkg_alpha_));  //��(1)����ͼ���alpha͸��
            }

            alpha_cache_.insert(w, pixmap);
        }

        QPainter painter(w);
        //painter.setCompositionMode(QPainter::CompositionMode_Source);
        //������Ĭ�ϵ�SourceOverģʽ��alpha͸���ı���ͼ��Ȼ���ӿؼ�border-image����alpha͸���ˡ������㷨ԭ���ݲ���
        //ʵ����Ӧ���ǽ��ӿؼ���border-image���ó�alpha͸����������qss��border-image. ���������������鷳
        painter.drawPixmap(0, 0, *pixmap); 
    }

    return false;
}

void Base_Widget_Imp::set_child_bkg_alpha( int alpha )
{
    child_bkg_alpha_ = (alpha < 0) ? 0 : (alpha > 255 ? 255 : alpha);
}

//////////////////////////////////////////////////////////////////////////
void CenterParent( QWidget* parent, QWidget* child )
{
    QRect parentFrame = parent->frameGeometry();    //�����������Ļ����
    QRect childFrame  = child->frameGeometry();

    QPoint centerparent(             
                parentFrame.x() + ((parentFrame.width()  - childFrame.width()) /2),             
                parentFrame.y() + ((parentFrame.height() - childFrame.height()) /2)
                );

    QDesktopWidget * pDesktop = QApplication::desktop();     
    QRect sgRect = pDesktop->screenGeometry(pDesktop->screenNumber(parent));

    if (centerparent.x() < sgRect.left()){
        centerparent.setX(sgRect.left());     
    }else if ((centerparent.x() + childFrame.width()) > sgRect.right()) {
        centerparent.setX(sgRect.right() - childFrame.width()); 
    }

    if(centerparent.y() < sgRect.top()) {
        centerparent.setY(sgRect.top());     
    }else if((centerparent.y() + childFrame.height()) > sgRect.bottom()) {
        centerparent.setY(sgRect.bottom() - childFrame.height()); 
    }

    QPoint pt = child->mapFromGlobal(centerparent);
    child->move(pt);
}

