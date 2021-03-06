/*
    IanniX — a graphical real-time open-source sequencer for digital art
    Copyright (C) 2010-2012 — IanniX Association

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NXCURSOR_H
#define NXCURSOR_H

#include <QEasingCurve>
#include "objects/nxcurve.h"
#include "objects/nxtrigger.h"
#include "geometry/nxeasing.h"

#ifndef M_PI
#define M_PI    (3.14159265358979323846)
#endif
#ifndef M_PI_2
#define M_PI_2	(1.57079632679489661923)
#endif


class NxCursor : public NxObject {
    Q_OBJECT
    Q_PROPERTY(QString start             READ getStart              WRITE setStart)
    Q_PROPERTY(qreal   timeStartOffset   READ getTimeStartOffset    WRITE setTimeStartOffset)
    Q_PROPERTY(qreal   timeInitialOffset READ getTimeInitialOffset  WRITE setTimeInitialOffset)
    Q_PROPERTY(qreal   timeEndOffset     READ getTimeEndOffset      WRITE setTimeEndOffset)
    Q_PROPERTY(QString boundsSource      READ getBoundsSource       WRITE setBoundsSource)
    Q_PROPERTY(QString boundsTarget      READ getBoundsTarget       WRITE setBoundsTarget)
    Q_PROPERTY(qreal   timeFactor        READ getTimeFactor         WRITE setTimeFactor)
    Q_PROPERTY(qreal   timeFactorAuto    READ getTimeFactorAuto     WRITE setTimeFactorAuto)
    Q_PROPERTY(qreal   timeFactorF       READ getTimeFactorF        WRITE setTimeFactorF)
    Q_PROPERTY(qreal   cursorWidth       READ getWidth              WRITE setWidth)
    Q_PROPERTY(qreal   cursorDepth       READ getDepth              WRITE setDepth)
    Q_PROPERTY(quint16 easingStart       READ getEasing             WRITE setEasing)
    Q_PROPERTY(quint16 nbLoop            READ getNbLoop             WRITE setNbLoop)
    Q_PROPERTY(qreal   timeLocal         READ getTimeLocal          WRITE setTimeLocal)
    Q_PROPERTY(qreal   timeLocalPercent  READ getTimeLocalPercent   WRITE setTimeLocalPercent)
    Q_PROPERTY(bool    forceTrig         READ getForceTrig          WRITE setForceTrig)


public:
    explicit NxCursor(NxObjectFactoryInterface *parent, QTreeWidgetItem *ccParentItem, UiRenderOptions *_renderOptions);
    ~NxCursor();

private:
    NxCurve *curve;
private:
    qreal factors, timeLocal, timeLocalOld, timeLocalAbsolute, time, timeOld, nextTimeOld;
    qreal timeLocalLastSend, timeLastSend;
    qreal timeStartOffset, timeEndOffset, timeInitialOffset;
    NxEasing easing;
    qreal timeFactor, timeFactorF;
    quint16 nbLoop, nbLoopOld;
    qreal width, depth;
    NxRect boundsSource, boundsTarget;
    bool boundsSourceIsBoundingRect;
    bool previousCursorReliable, previousPreviousCursorReliable;
    QVector<qreal> start;
    //NxLine cursor, cursorOld;
    NxPolygon cursorPoly, cursorPolyOld;
    NxPoint cursorPosOld, cursorAngleOld;
    NxPoint cursorPos, cursorRelativePos, cursorAngle;
    NxPoint cursorPosLastSend, cursorRelativePosLastSend, cursorAngleLastSend;
    qreal cursorAngleCacheSinZ, cursorAngleCacheCosZ, cursorAngleCacheSinY, cursorAngleCacheCosY;
    GLuint glListCursor;
public:
    inline quint8 getType() const {
        return ObjectsTypeCursor;
    }
    inline const QString getTypeStr() const {
        return "cursor";
    }

    inline void dragStart(const NxPoint &) {
        posDrag = pos;
        isDrag = true;
    }
    inline void drag(const NxPoint & translation, const NxPoint & mousePos) {
        if(curve) {
            setTimeLocalPercent(curve->intersects(NxRect(mousePos - NxPoint(renderOptions->objectSize, renderOptions->objectSize), mousePos + NxPoint(renderOptions->objectSize, renderOptions->objectSize))));
            factory->timerTrig(this, true);
        }
        else
            setPos(posDrag + translation);
    }

    inline QString getPosStr() const {
        return QString("%1 %2 %3").arg(cursorPos.x()).arg(cursorPos.y()).arg(cursorPos.z());
    }

    void setTime(qreal delta);

    inline void setTimeLocal(qreal _timeLocal) {
        timeLocal = _timeLocal * timeFactor;
        timeLocalAbsolute = qAbs(timeLocal);
        timeLocalOld = timeLocal;
        setNbLoop(0);
        setTime(0);
    }
    inline qreal getTimeLocal() const {
        return timeLocal;
    }
    inline qreal getTimeLocalLastSend() const {
        return timeLocalLastSend;
    }
    inline void setTimeLocalPercent(qreal _timeLocalPercent) {
        if((curve) && (0 <= _timeLocalPercent) && (_timeLocalPercent <= 1)) {
            timeLocal = _timeLocalPercent * curve->getPathLength();
            timeLocalAbsolute = qAbs(timeLocal);
            timeLocalOld = timeLocal;
            setNbLoop(0);
            setTime(0);
        }
    }
    inline qreal getTimeLocalPercent() const {
        return time;
    }
    inline qreal getTimeLocalPercentLastSend() const {
        return timeLastSend;
    }

    inline void setStart(const QString & startStr) {
        QStringList startItems = startStr.split(" ", QString::SkipEmptyParts);
        setNbLoop(0);
        setTime(0);
        start.clear();
        foreach(const QString & startItem, startItems)
            start.append(startItem.toDouble());
    }
    inline const QString getStart() const {
        QString retour;
        foreach(const qreal startItem, start)
            retour += QString::number(startItem) + " ";
        return retour.trimmed();
    }

    inline void setNbLoop(quint16 _nbLoop) {
        nbLoop = _nbLoop;
    }
    inline quint16 getNbLoop() const {
        return nbLoop;
    }
    inline void setEasing(quint16 _easing) {
        easing.setType(_easing);
    }
    inline quint16 getEasing() const {
        return easing.getType();
    }

    inline void setTimeStartOffset(qreal _timeStartOffset) {
        timeStartOffset = _timeStartOffset;
        setTime(0);
    }
    inline qreal getTimeStartOffset() const {
        return timeStartOffset;
    }
    inline void setTimeEndOffset(qreal _timeEndOffset) {
        timeEndOffset = _timeEndOffset;
        setTime(0);
    }
    inline qreal getTimeEndOffset() const {
        return timeEndOffset;
    }
    inline void setTimeInitialOffset(qreal _timeInitialOffset) {
        timeInitialOffset = _timeInitialOffset;
        setTime(0);
    }
    inline qreal getTimeInitialOffset() const {
        return timeInitialOffset;
    }

    inline const NxPoint getCursorValue(const NxPoint & _pos) const {
        NxRect _boundsSource = boundsSource;
        if(_boundsSource.width() == 0)  _boundsSource.setWidth(0.0001);
        if(_boundsSource.height() == 0) _boundsSource.setHeight(0.0001);
        if(_boundsSource.length() == 0) _boundsSource.setLength(0.0001);
        return NxPoint( ((_pos.x() - _boundsSource.left())   / _boundsSource.width())  * boundsTarget.width()  + boundsTarget.left() ,
                        ((_pos.y() - _boundsSource.bottom()) / _boundsSource.height()) * boundsTarget.height() + boundsTarget.bottom(),
                        ((_pos.z() - _boundsSource.zTop())   / _boundsSource.length()) * boundsTarget.length() + boundsTarget.zTop() );
    }


    inline void setBoundsRect(quint16 index, qreal val, bool source) {
        NxRect *boundsRect;
        if(source) {
            boundsRect = &boundsSource;
            boundsSourceIsBoundingRect = false;
        }
        else
            boundsRect = &boundsTarget;
        if(index == 0)      boundsRect->setTopLeft(NxPoint(val, boundsRect->topLeft().y(), boundsRect->topLeft().z()));
        else if(index == 1) boundsRect->setTopLeft(NxPoint(boundsRect->topLeft().x(), val, boundsRect->topLeft().z()));
        else if(index == 2) boundsRect->setTopLeft(NxPoint(boundsRect->topLeft().x(), boundsRect->topLeft().y(), val));
        else if(index == 3) boundsRect->setBottomRight(NxPoint(val, boundsRect->bottomRight().y(), boundsRect->bottomRight().z()));
        else if(index == 4) boundsRect->setBottomRight(NxPoint(boundsRect->bottomRight().x(), val, boundsRect->bottomRight().z()));
        else if(index == 5) boundsRect->setBottomRight(NxPoint(boundsRect->bottomRight().x(), boundsRect->bottomRight().y(), val));
    }
    inline qreal getBoundsRect(quint16 index, bool source) const {
        NxRect boundsRect;
        if(source)
            boundsRect = boundsSource;
        else
            boundsRect = boundsTarget;
        if(index == 0)      return boundsRect.topLeft().x();
        else if(index == 1) return boundsRect.topLeft().y();
        else if(index == 2) return boundsRect.topLeft().z();
        else if(index == 3) return boundsRect.bottomRight().x();
        else if(index == 4) return boundsRect.bottomRight().y();
        else if(index == 5) return boundsRect.bottomRight().z();
        return 0;
    }

    inline void setBoundsRectStr(const QString & _bounds, bool source, quint16 part = 2) {
        QStringList bounds = _bounds.split(" ", QString::SkipEmptyParts);
        if(part == 0) {
            if(bounds.count() == 2) {
                setBoundsRect(0, bounds.at(0).toDouble(), source);
                setBoundsRect(1, bounds.at(1).toDouble(), source);
            }
            else if(bounds.count() == 3) {
                setBoundsRect(0, bounds.at(0).toDouble(), source);
                setBoundsRect(1, bounds.at(1).toDouble(), source);
                setBoundsRect(2, bounds.at(2).toDouble(), source);
            }
        }
        else if(part == 1) {
            if(bounds.count() == 2) {
                setBoundsRect(3, bounds.at(0).toDouble(), source);
                setBoundsRect(4, bounds.at(1).toDouble(), source);
            }
            else if(bounds.count() == 3) {
                setBoundsRect(3, bounds.at(0).toDouble(), source);
                setBoundsRect(4, bounds.at(1).toDouble(), source);
                setBoundsRect(5, bounds.at(2).toDouble(), source);
            }
        }
        else {
            if(bounds.count() == 4) {
                setBoundsRect(0, bounds.at(0).toDouble(), source);
                setBoundsRect(1, bounds.at(1).toDouble(), source);
                setBoundsRect(3, bounds.at(2).toDouble(), source);
                setBoundsRect(4, bounds.at(3).toDouble(), source);
            }
            else if(bounds.count() == 6) {
                setBoundsRect(0, bounds.at(0).toDouble(), source);
                setBoundsRect(1, bounds.at(1).toDouble(), source);
                setBoundsRect(2, bounds.at(2).toDouble(), source);
                setBoundsRect(3, bounds.at(3).toDouble(), source);
                setBoundsRect(4, bounds.at(4).toDouble(), source);
                setBoundsRect(5, bounds.at(5).toDouble(), source);
            }
        }
    }
    inline const QString getBoundsRectStr(bool source, quint16 part = 2) const {
        if(part == 0)
            return QString("%1 %2 %3").arg(getBoundsRect(0, source), 0, 'f', 3).arg(getBoundsRect(1, source), 0, 'f', 3).arg(getBoundsRect(2, source), 0, 'f', 3);
        else if(part == 1)
            return QString("%1 %2 %3").arg(getBoundsRect(3, source), 0, 'f', 3).arg(getBoundsRect(4, source), 0, 'f', 3).arg(getBoundsRect(5, source), 0, 'f', 3);
        else
            return QString("%1 %2 %3 %4 %5 %6").arg(getBoundsRect(0, source), 0, 'f', 3).arg(getBoundsRect(1, source), 0, 'f', 3).arg(getBoundsRect(2, source), 0, 'f', 3).arg(getBoundsRect(3, source), 0, 'f', 3).arg(getBoundsRect(4, source), 0, 'f', 3).arg(getBoundsRect(5, source), 0, 'f', 3);
    }


    inline void setBoundsSource(const QString & bounds, quint16 part = 2) {
        setBoundsRectStr(bounds, true, part);
    }
    inline void setBoundsTarget(const QString & bounds, quint16 part = 2) {
        setBoundsRectStr(bounds, false, part);
    }
    inline const QString getBoundsSource(quint16 part = 2) const {
        return getBoundsRectStr(true, part);
    }
    inline const QString getBoundsTarget(quint16 part = 2) const {
        return getBoundsRectStr(false, part);
    }


    inline void setTimeFactor(qreal _timeFactor) {
        timeFactor = _timeFactor;
    }
    inline qreal getTimeFactor() const {
        return timeFactor;
    }
    inline void setTimeFactorAuto(qreal _timeFactorAuto) {
        if((curve) && (_timeFactorAuto))
            timeFactor = curve->getPathLength() / _timeFactorAuto;
    }
    inline qreal getTimeFactorAuto() const {
        return timeFactor;
    }
    inline void setTimeFactorF(qreal _timeFactorF) {
        timeFactorF = _timeFactorF;
    }
    inline qreal getTimeFactorF() const {
        return timeFactorF;
    }

    inline void setWidth(qreal _width) {
        width = _width;
        calculate();
    }
    inline qreal getWidth() const {
        return width;
    }

    inline void setDepth(qreal _depth) {
        depth = _depth;
        calculate();
    }
    inline qreal getDepth() const {
        return depth;
    }

    inline NxCurve* getCurve() const {
        return curve;
    }
    inline void setCurve(NxCurve *_curve) {
        if(curve)
            curve->removeCursor(this);
        curve = _curve;
        if(curve)
            curve->addCursor(this);
        calculate();
    }

    bool contains(NxTrigger *trigger) const;
    bool trig(NxCurve *collisionCurve);

    inline void calcBoundingRect() {
        //Bounding rect + margin
        boundingRect = cursorPoly.boundingRect();
        boundingRect = boundingRect.normalized();
        if((boundsSourceIsBoundingRect) && (curve))
            boundsSource = NxRect(curve->getBoundingRect().normalized().bottomLeft(), curve->getBoundingRect().normalized().topRight());
    }
    inline bool isMouseHover(const NxPoint & mouse) {
        NxRect mouseAdjusted = NxRect(mouse - NxPoint(renderOptions->objectSize/2, renderOptions->objectSize/2), mouse + NxPoint(renderOptions->objectSize/2, renderOptions->objectSize/2));
        if(mouseAdjusted.contains(cursorPos))
            return true;
        else
            return false;
        /*
        if((cursor.intersect(NxLine(mouseAdjusted.topLeft(), mouseAdjusted.bottomRight()), 0) == NxLine::BoundedIntersection) || (cursor.intersect(NxLine(mouseAdjusted.bottomLeft(), mouseAdjusted.topRight()), 0) == NxLine::BoundedIntersection))
            return true;
        else
            return false;
        */
    }


public:
    QString serializeCustom(bool hasAScript) const {
        QString retour = "";
        QString prefix = "", postfix = COMMAND_END;
        if(hasAScript) {
            prefix = "run(\"";
            postfix =  "\");" + COMMAND_END;
        }
        if(curve) {
            retour += prefix + QString(COMMAND_CURSOR_CURVE + " %1 %2").arg("current").arg("lastCurve") + postfix;
            if((getLineStipple() != 0xFFFF) || (getLineFactor() != 1))
                retour += prefix + QString(COMMAND_LINE + " %1 %2 %3").arg("current").arg(getLineStipple()).arg(getLineFactor()) + postfix;
            if((getTimeInitialOffset() != 0) || (getTimeStartOffset() != 0) || (getTimeEndOffset() != 0))
                retour += prefix + QString(COMMAND_CURSOR_OFFSET + " %1 %2 %3 %4").arg("current").arg(getTimeInitialOffset()).arg(getTimeStartOffset()).arg(getTimeEndOffset()) + postfix;
            if(!boundsSourceIsBoundingRect)
                retour += prefix + QString(COMMAND_CURSOR_BOUNDS_SOURCE + " %1 %2").arg("current").arg(getBoundsSource()) + postfix;
            if((getBoundsTarget() != "0.000 1.000 0.000 1.000 0.000 1.000") && (getBoundsTarget() != "0.000 1.000 1.000 0.000"))
                retour += prefix + QString(COMMAND_CURSOR_BOUNDS_TARGET + " %1 %2").arg("current").arg(getBoundsTarget()) + postfix;
            if(getTimeFactor() != 1)
                retour += prefix + QString(COMMAND_CURSOR_SPEED + " %1 %2").arg("current").arg(getTimeFactor()) + postfix;
            if(getTimeFactorF() != 1)
                retour += prefix + QString(COMMAND_CURSOR_SPEEDF + " %1 %2").arg("current").arg(getTimeFactorF()) + postfix;
            if(getWidth() != 1)
                retour += prefix + QString(COMMAND_CURSOR_WIDTH + " %1 %2").arg("current").arg(getWidth()) + postfix;
            if(getDepth() != 0)
                retour += prefix + QString(COMMAND_CURSOR_DEPTH + " %1 %2").arg("current").arg(getWidth()) + postfix;
            if((getEasing() != 0) || (getStart() != "1 0"))
                retour += prefix + QString(COMMAND_CURSOR_START + " %1 %2 %3 %4").arg("current").arg(getEasing()).arg(0).arg(getStart()) + postfix;
            /*
            if(getTimeLocal() != 0)
                retour += prefix + QString(COMMAND_CURSOR_TIME + " %1 %2").arg("current").arg(getTimeLocal()) + postfix;
                */
            if(getTimeLocalPercent() != 0)
                retour += prefix + QString(COMMAND_CURSOR_TIME_PERCENT + " %1 %2").arg("current").arg(getTimeLocalPercent()) + postfix;

            if(!colorActive.isEmpty()) {
                if(getColorActive() != "cursor_active")
                    retour += prefix + QString(COMMAND_COLOR_ACTIVE + " %1 %2").arg("current").arg(getColorActive()) + postfix;
            }
            else
                retour += prefix + QString(COMMAND_COLOR_ACTIVE + " %1 %2 %3 %4 %5").arg("current").arg(getColorActiveColor().red()).arg(getColorActiveColor().green()).arg(getColorActiveColor().blue()).arg(getColorActiveColor().alpha()) + postfix;
            if(!colorInactive.isEmpty()) {
                if(getColorInactive() != "cursor_inactive")
                    retour += prefix + QString(COMMAND_COLOR_INACTIVE + " %1 %2").arg("current").arg(getColorInactive()) + postfix;
            }
            else
                retour += prefix + QString(COMMAND_COLOR_INACTIVE + " %1 %2 %3 %4 %5").arg("current").arg(getColorInactiveColor().red()).arg(getColorInactiveColor().green()).arg(getColorInactiveColor().blue()).arg(getColorInactiveColor().alpha()) + postfix;
            if(getSize() != 2)
                retour += prefix + QString(COMMAND_SIZE + " %1 %2").arg("current").arg(getSize()) + postfix;
            if((messageTimeInterval != 20) || (getMessagePatternsStr() != QSettings().value("defaultMessageCursor").toString()+" ,"))
                retour += prefix + QString(COMMAND_MESSAGE + " %1 %2, %3").arg("current").arg(QString::number(messageTimeInterval)).arg(getMessagePatternsStr()) + postfix;
        }
        return retour;
    }


public:
    void calculate();
    inline const NxPolygon & getCurrentPolygon() const {
        return cursorPoly;
    }
    /*
    inline qreal getCurrentPosition() const {
        if(curve)   return time * curve->getPathLength() / qAbs(factors);
        else        return 0;
    }
    */
    inline const NxPoint & getCurrentValue() const {
        return cursorRelativePos;
    }
    inline const NxPoint & getCurrentAngle() const {
        return cursorAngle;
    }
    inline const NxPoint & getCurrentPos() const {
        return cursorPos;
    }
    inline const NxPoint & getCurrentValueLastSend() const {
        return cursorRelativePosLastSend;
    }
    inline const NxPoint & getCurrentAngleLastSend() const {
        return cursorAngleLastSend;
    }
    inline const NxPoint & getCurrentPosLastSend() const {
        return cursorPosLastSend;
    }


public:
    void paint();
    void trig(bool force = false);
    bool getForceTrig() { return false;}
    void setForceTrig(bool val) {
        factory->timerTrig(this, val);
    }


private:
    inline qreal fmod(qreal a, qreal b, qint32 *nb) {
        if(b)
            *nb = (qint32)floor(a / b);
        else
            *nb = 0;
        return a - (qreal)(*nb) * b;
    }
};
#endif // NXCURSOR_H
