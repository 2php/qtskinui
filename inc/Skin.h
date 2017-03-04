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

#ifndef _SKIN_H_
#define _SKIN_H_

#include <QScopedPointer>
#include <QPixmap>
#include <QMargins>

//�߿�ͼƬ������һ�ű���ͼ, qss��border-width, qss��margin
class Border_Image
{
public:
    const QMargins& margin() const {return margin_;}
    const QMargins& border() const {return border_;}
    const QPixmap&  pixmap() const {return pixmap_;}
    const QString&  pixmap_url() const {return pixmap_url_;}

public:
    void set_pixmap(const QString& url);
    void set_pixmap(const QPixmap& pixmap);

    //order: left top right bottom
    void set_border(const QString& border); 
    void set_border(const QMargins& border);
    void set_border(int left, int top, int right, int bottom);

    void set_margin(const QString& border);
    void set_margin(const QMargins& border);
    void set_margin(int left, int top, int right, int bottom);

    void load(const QString& pixmap_url, const QString& border, const QString& margin);

private:
    QMargins margin_;
    QMargins border_;
    QPixmap  pixmap_;
    QString  pixmap_url_;
};

class QSettings;
class Skin
{
public:
    Skin();
    ~Skin();

public:
    //Ӧ�ó���ͼ��
    const QPixmap& app_pixmap() const {return app_pixmap_;}
    void set_app_pixmap(const QPixmap& pixmap) {app_pixmap_ = pixmap;}

    //��С����ť,4��״̬
    const QPixmap& min_pixmap() const {return min_pixmap_;}
    void set_min_pixmap(const QPixmap& pixmap){min_pixmap_ = pixmap;}

    //��󻯰�ť,4��״̬
    const QPixmap& max_pixmap() const {return max_pixmap_;}
    void set_max_pixmap(const QPixmap& pixmap){max_pixmap_ = pixmap;}

    //�ָ���ť,4��״̬
    const QPixmap& restore_pixmap() const {return restore_pixmap_;}
    void set_restore_pixmap(const QPixmap& pixmap){restore_pixmap_ = pixmap;}
    
    //�رհ�ť,4��״̬
    const QPixmap& close_pixmap() const {return close_pixmap_;}
    void set_close_pixmap(const QPixmap& pixmap){close_pixmap_ = pixmap;}
    
    //������ť��4��״̬
    const QPixmap& skin_pixmap() const {return skin_pixmap_;}
    void set_skin_pixmap(const QPixmap& pixmap){skin_pixmap_ = pixmap;}

    //�˵���ť��4��״̬
    const QPixmap& menu_pixmap() const {return menu_pixmap_;}
    void set_menu_pixmap(const QPixmap& pixmap){menu_pixmap_ = pixmap;}
    
    //�����QSS,����һ��label
    const QString& titlebar_qss() const {return titlebar_qss_;}
    void set_titlebar_qss(const QString& qss){titlebar_qss_ = qss;}

    //�������ı���
    const Border_Image& titlebar_background() const {return titlebar_background_;}
    Border_Image& titlebar_background() {return titlebar_background_;}

    //�������߶�. -1: ����Ϊ����ͼƬ�ĸ߶�; 0: ����Ϊ��ǰ��ʽ�������ĸ߶�: 1~N: �߶ȼ�Ϊ��ֵ
    int titlebar_height() const {return titlebar_height_;}
    void set_titlebar_height(int h) {titlebar_height_ = h;}

    //titlebar��ʽ, ָ����ʾ��Щ��ť
    int titlebar_showstyle() const {return titlebar_showstyle_;}
    void set_titlebar_showstyle(int s){titlebar_showstyle_ = s;}

    //Base_Widget�ڴ�����Ĭ��ʹ�õ�qss, ����������������
    const QString& widget_qss() const {return widget_qss_;}
    void set_widget_qss(const QString& qss){widget_qss_ = qss;}

    //����
    inline const Border_Image& background() const {return background_;}
    inline Border_Image& background() {return background_;}

    //�ͻ�������ͼ
    inline const QPixmap& client_pixmap() const {return client_pixmap_;}
    QPixmap& client_pixmap() {return client_pixmap_;}
    void set_client_pixmap(const QPixmap& pixmap);
    void set_client_pixmap(const QString& url);

    //�ͻ����������Ʒ�ʽ
    inline int client_draw_type() const {return client_draw_type_;}
    void set_client_draw_type(int v);

    //Բ��
    int round_radius_x() const {return round_radius_x_;}
    int round_radius_y() const {return round_raidus_y_;}
    void set_round_radius(int x, int y);
    void set_round_radius(const QString& radius);

public:
    //��ȡƤ��. Ĭ�ϴ�skin.ini�ж�ȡdefault����Ϊskin
    virtual void load();

    //��ini�ж�ȡƤ������
    void load_from_ini(const QString& ini_file_name, const QString& section);

    //�������ļ��ж�ȡSkin, ����Skin����(�ɵ�����ɾ��)
    //@ini_file_name, ini�����ļ�; @section, ini�ļ��еĽ�
    static Skin* read_config_skin(const QString& ini_file_name, const QString& section);

protected:
    void load_pixmap(QSettings* ini, QPixmap& pixmap, const QString& key);
    void load_border_image(QSettings* ini, Border_Image& bi, 
                           const QString& bg_key, const QString& border_key, 
                           const QString& margin_key);
    void load_int(QSettings* ini, int& i, const QString& key);
    void load_string(QSettings* ini, QString& str, const QString& key);

protected:
    //ͼƬ��Դ
    QPixmap      app_pixmap_;
    QPixmap      min_pixmap_;
    QPixmap      max_pixmap_;
    QPixmap      restore_pixmap_;
    QPixmap      close_pixmap_;
    QPixmap      skin_pixmap_;
    QPixmap      menu_pixmap_;

    //����QSS,QLabel
    QString      titlebar_qss_;
    //titlebar�߿�ͼ
    Border_Image titlebar_background_;
    //titlebar�߶�
    int          titlebar_height_;
    //titlebar��ʽ, ָ����ʾ��Щ��ť
    int          titlebar_showstyle_;

    //����ͼ, �൱��frame
    Border_Image background_;
    //Base_Widget�ڴ�����Ĭ��ʹ�õ�qss, ����һЩ�������ԣ��磺����
    QString      widget_qss_;
    //�ͻ�������ͼƬ
    QPixmap      client_pixmap_;
    //�ͻ�������ͼƬ���췽ʽ�� 1-�����Ϲ̶����������죻2-���Ϲ̶�����������; 3-��ͼƬ��������������(���ܻᵼ�±���ʧ��!)
    int          client_draw_type_;

    //Բ��
    int         round_radius_x_, round_raidus_y_;
};

class Skin_Manager: public QObject
{
public:
    Skin_Manager(QObject *parent=0);
    ~Skin_Manager();

public:
    //��ini�ļ��м���skin
    void store(const QString& name, const QString& ini_file_name, const QString& section);
    void store(const QString& name, Skin* skin);
    void store(const QString& name, const QString& section);

    Skin* remove(const QString& name);
    void  erase(const QString& name);
    void  clear();

    Skin* default_skin();         //ini�ļ���[default]������skin
    Skin* msgbox_skin();          //ini�ļ���[msgbox]��������skin
    Skin* get(const QString& skin_name);

    static Skin_Manager* instance();

private:
    QHash<QString, Skin*> skin_hash_;
    QString               ini_file_;
};

#define skinman Skin_Manager::instance()

#endif
