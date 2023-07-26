#ifndef SLIDEDIALOG_H
#define SLIDEDIALOG_H

#include "slidepage.h"

class SlideDialog : public SlidePage
{
    Q_OBJECT

protected:
    void resizeEvent(QResizeEvent *event);

public:
    const int preferWidth = 200;
    const int preferHeight = 350;

    explicit SlideDialog(int radius, QString name, QWidget *parent = nullptr);
public slots:
    void slideIn();
    void slideOut();
};

#endif // SLIDEDIALOG_H
