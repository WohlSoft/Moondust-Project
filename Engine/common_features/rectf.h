/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PGE_RECTF_H
#define PGE_RECTF_H

class PGE_Rect;
class PGE_PointF;
class PGE_SizeF;

class PGE_RectF
{
        friend class PGE_Rect;
    public:
        PGE_RectF();
        PGE_RectF(double x, double y, double w, double h);
        PGE_RectF(const PGE_RectF &r) = default;
        PGE_RectF(const PGE_Rect &r);
        ~PGE_RectF();
        void setRect(double x, double y, double w, double h);
        void setPos(double x, double y);
        void setSize(double w, double h);

        void setLeft(double l);
        void setRight(double r);
        void setTop(double t);
        void setBottom(double b);
        PGE_RectF withMargin(double margin) const;
        PGE_RectF withMargin(double marginX, double marginY) const;

        void setX(double x);
        void setY(double y);
        void setWidth(double w);
        void setHeight(double h);
        void addX(double x);
        void addY(double y);

        void setTopLeft(PGE_PointF p);
        void setTopRight(PGE_PointF p);
        void setBottomRight(PGE_PointF p);
        void setBottomLeft(PGE_PointF p);

        void setTopLeft(double l, double t);
        void setTopRight(double r, double t);
        void setBottomRight(double r, double b);
        void setBottomLeft(double l, double b);

        double x() const;
        double y() const;

        double left() const;
        double top() const;
        double bottom() const;
        double right() const;

        double width() const;
        double height() const;

        PGE_PointF center() const;
        PGE_SizeF size() const;
        double centerX() const;
        double centerY() const;

        bool collidePoint(double x, double y) const;
        bool collideRect(double x, double y, double w, double h) const;
        bool collideRect(PGE_RectF &rect) const;
        bool collideRectDeep(PGE_RectF &rect, double depth) const;
        bool collideRectDeep(PGE_RectF &rect, double deepX, double deepY) const;
        bool collideRect(PGE_Rect &rect) const;

    private:
        double m_x;
        double m_y;
        double m_w;
        double m_h;
        double m_r;
        double m_b;
};

#endif // PGE_RECTF_H
