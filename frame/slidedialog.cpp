#include "slidedialog.h"

SlideDialog::SlideDialog(int radius, QString name, QWidget *parent) :
    SlidePage(radius,name, parent)
{
    //if(parent)
    //    resize(parent->width() * 0.8 <= preferWidth ? parent->width() * 0.8 : preferWidth, parent->height());
    resize(parent->width() * 0.4 <= preferWidth ? preferWidth : parent->width() * 0.4, parent->height() * 0.4 <= preferHeight ? preferHeight : parent->height() * 0.4);
    this->move((parent->width()-this->width())/2, -this->height() - 30);
    this->setParent(parent);
}

void SlideDialog::resizeEvent(QResizeEvent *event){
    bgWidget->resize(this->size());
    sheildLayer->resize(this->parentWidget()->size());
    if(!onShown && !curAni){
        this->move(QPoint((this->parentWidget()->width()-this->width())/2, -this->height() - 30));}
    else if(!onShown && curAni)
        emit sizeChange();
    else
        this->move(QPoint((this->parentWidget()->width()-this->width())/2, 0));

}

void SlideDialog::slideIn(){
    if(curAni){
        curAni->stop();
        curAni->deleteLater();
        curAni = nullptr;
    }
    onShown = true;
    sheildLayer->raise();
    sheildLayer->setEnabled(true);
    this->raise();
    sheildLayer->show();
    QParallelAnimationGroup *inGroup = new QParallelAnimationGroup(this);
    QPropertyAnimation *slideInAni = new QPropertyAnimation(this, "pos", this);
    slideInAni->setStartValue(this->pos());
    slideInAni->setEndValue(QPoint(this->pos().x(), 0));
    slideInAni->setDuration(500);
    slideInAni->setEasingCurve(QEasingCurve::InOutExpo);
    QPropertyAnimation *fadeInAni = new QPropertyAnimation(opacity, "opacity", this);
    fadeInAni->setStartValue(opacity->opacity());
    //> note: DO NOT CHANGE 0.99 TO 1!!!!!
    //>       Will cause unexpected position shift (maybe qt's bug)
    fadeInAni->setEndValue(0.99);
    fadeInAni->setDuration(500);
    QSequentialAnimationGroup *rotate = new QSequentialAnimationGroup(this);
    QPropertyAnimation *rotateAni = new QPropertyAnimation(backIcon, "rotationAngle", this);
    rotateAni->setStartValue(180);
    rotateAni->setEndValue(360);
    rotateAni->setDuration(500);
    rotateAni->setEasingCurve(QEasingCurve::InOutExpo);
    rotate->addPause(250);
    rotate->addAnimation(rotateAni);
    inGroup->addAnimation(slideInAni);
    inGroup->addAnimation(fadeInAni);
    inGroup->addAnimation(rotate);
    connect(inGroup, &QParallelAnimationGroup::finished, this, [=](){this->curAni = nullptr;});
    inGroup->start();
    curAni = inGroup;

    emit SlideInSignal();
}

void SlideDialog::slideOut(){
    if(curAni){
        curAni->stop();
        curAni->deleteLater();
        curAni = nullptr;
    }
    onShown = false;
    sheildLayer->setEnabled(false);
    QParallelAnimationGroup *outGroup = new QParallelAnimationGroup(this);
    QPropertyAnimation *slideOutAni = new QPropertyAnimation(this, "pos", this);
    slideOutAni->setStartValue(this->pos());
    slideOutAni->setEndValue(QPoint(this->pos().x(),  -this->height() - 30));
    slideOutAni->setDuration(500);
    slideOutAni->setEasingCurve(QEasingCurve::InOutExpo);
    QPropertyAnimation *fadeOutAni = new QPropertyAnimation(opacity, "opacity", this);
    fadeOutAni->setStartValue(opacity->opacity());
    fadeOutAni->setEndValue(0);
    fadeOutAni->setDuration(500);
    QPropertyAnimation *rotateAni = new QPropertyAnimation(backIcon, "rotationAngle", this);
    rotateAni->setStartValue(360);
    rotateAni->setEndValue(180);
    rotateAni->setDuration(500);
    rotateAni->setEasingCurve(QEasingCurve::InOutExpo);
    outGroup->addAnimation(slideOutAni);
    outGroup->addAnimation(fadeOutAni);
    outGroup->addAnimation(rotateAni);
    connect(outGroup, &QPropertyAnimation::finished, this, [=](){this->curAni = nullptr;pageContentContainer->scrollToTop();sheildLayer->hide();});
    connect(this, &SlidePage::sizeChange, slideOutAni, [=](){slideOutAni->setEndValue(QPoint(this->pos().x(),  -this->height() - 30));});
    outGroup->start();
    curAni = outGroup;

    emit SlideOutSignal();
}
