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

#ifndef _BASE_WIDGET_H_
#define _BASE_WIDGET_H_

#include <QtWidgets/QtWidgets>
#include <QWidget>
#include <QDialog>

#include "Skin.h"
#include "Title_Bar.h"

//class Title_Bar;
//class Skin;
class NcFramelessHelper;


class Base_Widget_Imp
{
public:
    Base_Widget_Imp(QWidget *owner, Skin *skin = NULL, bool is_dialog = false);
    ~Base_Widget_Imp();

public:
    //�Ƿ�����ͨ�����ı䴰�ڴ�С
    bool get_resizable() const {return resizable_;}
    void set_resizable(bool b);

    //overwrite
    bool isMaximized() const;
    QString windowTitle() const;
    void setWindowTitle(const QString &);

    //skin
    void load_skin(Skin* skin=NULL);

    //��ʾһ��Widget, ��Widget��Ϊ����
    void setup_widget(QWidget *content);

    //����Ի���λ��, 0�ɹ�������ֵʧ��
    int save_geometry(const QString& name);
    int save_geometry();

    //�ָ��Ի���λ��, 0�ɹ�������ֵʧ��
    int load_geometry(const QString& name);

    //����������Ļ����
    void move_center();

    //��ȥ�߿��Ŀͻ���rect
    QRect client_rect() const;

    //���õı���ͼƬ
    const QPixmap* bg_pixmap() const;

    //��һ�����������������ͻ�������
    QPoint window_to_client(const QPoint& pt);
    QRect  window_to_client(const QRect& rt);

    //��һ���ͻ��������������������
    QPoint client_to_window(const QPoint& pt);
    QRect  client_to_window(const QRect& rt);

    //�¼����ˣ�����paintEvent, �����Ӵ��ڶԱ���ͼ��alpha͸��
    bool eventFilter( QObject *o, QEvent *e );

    //�Ӵ��ڱ���alpha, 0-255(0:ȫ͸����255-��͸��)
    inline int child_bkg_alpha() const {return child_bkg_alpha_;}
    void set_child_bkg_alpha(int alpha); 

    //�������޸�
    void set_bg_changed() {
        recreate_bg_pixmap_ = true;
    }

public:
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);

private:
    void load_skin_internal();

    //������ͼ, ���Ͻǻ�ԭʼͼ�����½�����
    void draw_bg_topleft(QPainter *painter, const QRect& r, const QPixmap& img);

    //������ͼ, ���Ͻǻ�ԭʼͼ�����½�����
    void draw_bg_topright(QPainter *painter, const QRect& r, const QPixmap& img);

protected:
    int  init(bool is_dialog);    //��ʼ��

public:
    QVBoxLayout  *layout_;                      //����parentɾ��
    Title_Bar    *title_bar_;                   //����parentɾ��
    Skin         *skin_;                        //��ɾ��

private:
    bool               resize_temp_disabled_;   //��������󻯺��ֹ�϶��ı��С
    bool               resizable_;              //�Ƿ�����ı䴰�ڵĴ�С
    bool               recreate_bg_pixmap_;     //�Ƿ���Ҫ���´����ͻ���ͼ��
    QPixmap           *bg_pixmap_;              //���õı���ͼ��
    NcFramelessHelper *resize_helper_;
    QString            save_object_name_;       //������������ڽ����ڴ�С��λ�ñ���
    int                child_bkg_alpha_;         //�Ӵ��ڱ���alpha
    QHash<QObject*, QPixmap*> alpha_cache_;      //�����ӿؼ�alpha͸����ı���ͼ

private:
    QWidget *q;                                 //owner
};

//T can be QWidget or QDialog, 
//template class can not have Q_OBJECT, so this class can not have signal/slot
template<class T>
class Base_Widget: public T
{
public:
    Base_Widget(QWidget* parent=NULL, Skin* skin=NULL)
        :T(parent)
    {
        p = new Base_Widget_Imp(this, skin, inherits("QDialog")/*typeid(T) == typeid(QDialog)*/);
    }

    ~Base_Widget()
    {
        delete p;
    }

    typedef typename Base_Widget<T> BaseClass;

public:
    //�Ƿ�����ͨ�����ı䴰�ڴ�С
    bool get_resizable() const {return p->resizable_;}
    void set_resizable(bool b) {p->set_resizable(b);}

    //overwrite
    bool isMaximized() const {return p->isMaximized();}
    QString windowTitle() const {return p->windowTitle();}
    void setWindowTitle(const QString &s) {p->setWindowTitle(s);}

    //skin
    virtual void load_skin(Skin* skin=NULL) {p->load_skin(skin);}

    //������ʾ������. ��Qt Designer��ƶԻ���, Ȼ����ô˷������뵽layout_��
    template<typename UI>
    QWidget* setup_UI(UI* ui)
    {
        QWidget *content = new QWidget(this);   //��parentɾ��
        ui->setupUi(content);
        setup_widget(content);
        return content;
    }

    //��ʾһ��Widget, ��Widget��Ϊ����
    void setup_widget(QWidget *content) {p->setup_widget(content);}

    //����Ի���λ��, 0�ɹ�������ֵʧ��
    int save_geometry(const QString& name) {return p->save_geometry(name);}
    int save_geometry() {return p->save_geometry();}

    //�ָ��Ի���λ��, 0�ɹ�������ֵʧ��
    int load_geometry(const QString& name) {return p->load_geometry(name);}

    //����������Ļ����
    void move_center() {p->move_center();}

    //��ȥ�߿��Ŀͻ���rect
    QRect client_rect() const {return p->client_rect();}

    //����ͼƬ
    const QPixmap* bg_pixmap() const {return p->bg_pixmap();}

    //��һ�����������������ͻ�������
    QPoint window_to_client(const QPoint& pt) {return p->window_to_client(pt);}
    QRect  window_to_client(const QRect& rt) {return p->window_to_client(rt);}

    //��һ���ͻ��������������������
    QPoint client_to_window(const QPoint& pt) {return p->client_to_window(pt);}
    QRect  client_to_window(const QRect& rt) {return p->client_to_window(rt);}

    /*
    �¼����ˣ�����paintEvent, ���Ӵ�������ڱ���ͼƬalpha͸��
    �Ӵ���������һ����͸���ı���ͼ����installEventFilter(w), Ȼ�󴰿ڼ��ɵ����Ӵ�������ڱ�����͸����
    */
    bool eventFilter( QObject *o, QEvent *e )
    {
        if (alpha_children_.contains(o)) {
            if (p->eventFilter(o, e)) {
                return true;
            }
        }
        return T::eventFilter(o, e);
    }

    //�Ӵ��ڱ���alpha, 0-255(0:ȫ͸����255-��͸��)
    inline int child_bkg_alpha() const {p->child_bkg_alpha();}
    inline void set_child_bkg_alpha(int alpha) {p->set_child_bkg_alpha(alpha);}

    //���ӿؼ�@child�ı���alpha͸��. ����qssΪ@child����һ����͸����border-image
    void make_bkg_alpha(QWidget *child) 
    {
        alpha_children_.insert(child); 
        child->installEventFilter(this);
    }

    void redraw_background() {
        p->set_bg_changed();
        this->update();
    }

public:
    QVBoxLayout*  layout()    {return p->layout_;}
    Title_Bar*    title_bar() {return p->title_bar_;}
    Skin*         skin()      {return p->skin_;}

protected:
    void resizeEvent(QResizeEvent *e) {p->resizeEvent(e);}
    void paintEvent(QPaintEvent *e) {p->paintEvent(e);}

private:
    Base_Widget_Imp *p;
    QSet<QObject*> alpha_children_;
};

//��@child���е�@parent
void CenterParent(QWidget* parent, QWidget* child);


#endif

