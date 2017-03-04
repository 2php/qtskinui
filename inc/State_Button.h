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

#ifndef _STATE_BUTTON_H_
#define _STATE_BUTTON_H_

#include <QtWidgets/QPushButton>
#include <QPainter>
#include <QMouseEvent>

//a pixmap have 5 picture which state is normal\hover\pressed\disabled\checked
//this button does not draw focus rect

class State_Button : public QPushButton
{
	Q_OBJECT
public:
	explicit State_Button(QWidget *parent = 0);
	~State_Button();

public:
	void load_pximap(const QString& pic_name, int state_count=4);
    void set_pixmap(const QPixmap& pixmap, int state_count=4);

    void load_background(const QString& pic_name, int state_count=4);
    void set_background(const QPixmap& pixmap, int state_count=4);

protected:
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *);

private:
    void paint_pixmap();

private:
	//ö�ٰ�ť�ļ���״̬
	enum ButtonStatus {NORMAL, HOVER, PRESSED, DISABLED, CHECKED};
    //pximap_λͼ����, ����ѡ����һ. ������߶���Ҫ����ѡ��BACKGROUND + QPushButton::setIcon
    enum PixmapType   {NONE, FOREGROUND, BACKGROUND};

    QPixmap         pixmap_;        //ͼƬ
    PixmapType      pixmap_type_;
    int             state_count_;   //ͼƬ�м���״̬(������ͼ)
	ButtonStatus    status_;        //��ǰ״̬
	int             width_;         //��ť���
	int             height_;        //��ť�߶�
	bool            mouse_pressed_; //�������Ƿ���
};

//��ť��ͼ�������棬����������
class Icon_Text_Button: public QPushButton
{
public:
    Icon_Text_Button(QWidget *parent);
    ~Icon_Text_Button();

protected:
    void paintEvent(QPaintEvent *e);
};

//
class Common_Button : public State_Button
{
public:
    explicit Common_Button(QWidget *parent = 0);
};

#endif //
