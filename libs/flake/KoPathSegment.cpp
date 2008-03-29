/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoPathSegment.h"
#include "KoPathPoint.h"
#include <kdebug.h>
#include <QtGui/QPainterPath>
#include <QtGui/QMatrix>
#include <math.h>

class KoPathSegment::Private
{
public:
    Private( KoPathPoint * p1, KoPathPoint * p2 )
    {
        first = p1;
        second = p2;
    }

    KoPathPoint * first;
    KoPathPoint * second;
};

KoPathSegment::KoPathSegment( KoPathPoint * first, KoPathPoint * second )
    : d( new Private( first, second ) )
{
}

KoPathSegment::KoPathSegment( const KoPathSegment & segment )
    : d( new Private(0,0) )
{
   if( ! segment.first() || segment.first()->parent() )
       setFirst( segment.first() );
   else
       setFirst( new KoPathPoint( *segment.first() ) );

   if( ! segment.second() || segment.second()->parent() )
       setSecond( segment.second() );
   else
       setSecond( new KoPathPoint( *segment.second() ) );
}

KoPathSegment::KoPathSegment( const QPointF &p0, const QPointF &p1 )
    : d( new Private( new KoPathPoint(), new KoPathPoint() ) )
{
    d->first->setPoint( p0 );
    d->second->setPoint( p1 );
}

KoPathSegment::KoPathSegment( const QPointF &p0, const QPointF &p1, const QPointF &p2 )
    : d( new Private( new KoPathPoint(), new KoPathPoint() ) )
{
    d->first->setPoint( p0 );
    d->first->setControlPoint2( p1 );
    d->second->setPoint( p2 );
}

KoPathSegment::KoPathSegment( const QPointF &p0, const QPointF &p1, const QPointF &p2, const QPointF &p3 )
    : d( new Private( new KoPathPoint(), new KoPathPoint() ) )
{
    d->first->setPoint( p0 );
    d->first->setControlPoint2( p1 );
    d->second->setPoint( p3 );
    d->second->setControlPoint1( p2 );
}

KoPathSegment & KoPathSegment::operator=( const KoPathSegment &rhs )
{
    if( this == &rhs )
        return (*this);

    if( ! rhs.first() || rhs.first()->parent() )
        setFirst( rhs.first() );
    else
        setFirst( new KoPathPoint( *rhs.first() ) );

    if( ! rhs.second() || rhs.second()->parent() )
        setSecond( rhs.second() );
    else
        setSecond( new KoPathPoint( *rhs.second() ) );

    return (*this);
}

KoPathSegment::~KoPathSegment()
{
    if( d->first && ! d->first->parent() )
        delete d->first;
    if( d->second && ! d->second->parent() )
        delete d->second;
}

KoPathPoint * KoPathSegment::first() const
{
    return d->first;
}

void KoPathSegment::setFirst( KoPathPoint * first )
{
    if( d->first && ! d->first->parent() )
        delete d->first;
    d->first = first;
}

KoPathPoint * KoPathSegment::second() const
{
    return d->second;
}

void KoPathSegment::setSecond( KoPathPoint * second )
{
    if( d->second && ! d->second->parent() )
        delete d->second;
    d->second = second;
}

bool KoPathSegment::isValid() const
{
    return (d->first && d->second);
}

bool KoPathSegment::operator == ( const KoPathSegment &rhs ) const
{
    if( ! isValid() && ! rhs.isValid() )
        return true;
    if( isValid() && ! rhs.isValid() )
        return false;
    if( ! isValid() && rhs.isValid() )
        return false;

    return ( *first() == *rhs.first() &&  *second() == *rhs.second() );
}

int KoPathSegment::degree() const
{
    if( ! isValid() )
        return -1;

    bool c1 = d->first->activeControlPoint2();
    bool c2 = d->second->activeControlPoint1();
    if( ! c1 && ! c2 )
        return 1;
    if( c1 && c2 )
        return 3;
    return 2;
}

QPointF KoPathSegment::pointAt( qreal t ) const
{
    if( ! isValid() )
        return QPointF();

    if( degree() == 1 )
    {
        return d->first->point() + t * ( d->second->point() - d->first->point() );
    }
    else
    {
        QPointF splitP;

        deCasteljau( t, 0, 0, &splitP, 0, 0 );

        return splitP;
    }
}

QRectF KoPathSegment::controlPointRect() const
{
    if( ! isValid() )
        return QRectF();

    QList<QPointF> points = controlPoints();
    QRectF bbox( points.first(), points.first() );
    foreach( QPointF p, points )
    {
        bbox.setLeft( qMin( bbox.left(), p.x() ) );
        bbox.setRight( qMax( bbox.right(), p.x() ) );
        bbox.setTop( qMin( bbox.top(), p.y() ) );
        bbox.setBottom( qMax( bbox.bottom(), p.y() ) );
    }

    if( degree() == 1 )
    {
        // adjust bounding rect of horizontal and vertical lines
        if( bbox.height() == 0.0 )
            bbox.setHeight( 0.1 );
        if( bbox.width() == 0.0 )
            bbox.setWidth( 0.1 );
    }

    return bbox;
}

QRectF KoPathSegment::boundingRect() const
{
    if( ! isValid() )
        return QRectF();

    QRectF rect = QRectF( d->first->point(), d->second->point() ).normalized();

    if( degree() == 1 )
    {
        // adjust bounding rect of horizontal and vertical lines
        if( rect.height() == 0.0 )
            rect.setHeight( 0.1 );
        if( rect.width() == 0.0 )
            rect.setWidth( 0.1 );
    }
    else
    {
        /*
         * The basic idea for calculating the axis aligned bounding box (AABB) for bezier segments
         * was found in comp.graphics.algorithms:
         * Use the points at the extrema of the curve to calculate the AABB.
         */
        foreach( qreal t, extrema() )
        {
            if( t >= 0.0 && t <= 1.0 )
            {
                QPointF p = pointAt( t );
                rect.setLeft( qMin( rect.left(), p.x() ) );
                rect.setRight( qMax( rect.right(), p.x() ) );
                rect.setTop( qMin( rect.top(), p.y() ) );
                rect.setBottom( qMax( rect.bottom(), p.y() ) );
            }
        }
    }

    return rect;
}

QList<QPointF> KoPathSegment::intersections( const KoPathSegment &segment ) const
{
    // this function uses a technique known as bezier clipping to find the
    // intersections of the two bezier curves

    QList<QPointF> isects;

    if( ! isValid() || ! segment.isValid() )
        return isects;

    int degree1 = degree();
    int degree2 = segment.degree();

    QRectF myBound = boundingRect();
    QRectF otherBound = segment.boundingRect();
    //kDebug(30006) << "my boundingRect =" << myBound;
    //kDebug(30006) << "other boundingRect =" << otherBound;
    if( ! myBound.intersects( otherBound ) )
    {
        //kDebug(30006) << "segments do not intersect";
        return isects;
    }

    // short circuit lines intersection
    if( degree1 == 1 && degree2 == 1 )
    {
        //kDebug(30006) << "intersecting two lines";
        isects += linesIntersection( segment );
        return isects;
    }

    // first calculate the fat line L by using the signed distances
    // of the control points from the chord
    qreal dmin, dmax;
    if( degree1 == 1 )
    {
        dmin = 0.0;
        dmax = 0.0;
    }
    else if( degree1 == 2 )
    {
        qreal d1;
        if( d->first->activeControlPoint2() )
            d1 = distanceFromChord( d->first->controlPoint2() );
        else
            d1 = distanceFromChord( d->second->controlPoint1() );
        dmin = qMin( 0.0, 0.5*d1 );
        dmax = qMax( 0.0, 0.5*d1 );
    }
    else
    {
        qreal d1 = distanceFromChord( d->first->controlPoint2() );
        qreal d2 = distanceFromChord( d->second->controlPoint1() );
        if( d1*d2 > 0.0 )
        {
            dmin = 0.75 * qMin( 0.0, qMin( d1, d2 ) );
            dmax = 0.75 * qMax( 0.0, qMax( d1, d2 ) );
        }
        else
        {
            dmin = 4.0/9.0 * qMin( 0.0, qMin( d1, d2 ) );
            dmax = 4.0/9.0 * qMax( 0.0, qMax( d1, d2 ) );
        }
    }

    //kDebug(30006) << "using fat line: dmax =" << dmax << " dmin =" << dmin;

    /*
      the other segment is given as a bezier curve of the form:
     (1) P(t) = sum_i P_i * B_{n,i}(t)
     our chord line is of the form:
     (2) ax + by + c = 0
     we can determine the distance d(t) from any point P(t) to our chord
     by substituting formula (1) into formula (2):
     d(t) = sum_i d_i B_{n,i}(t), where d_i = a*x_i + b*y_i + c
     which forms another explicit bezier curve
     D(t) = (t,d(t)) = sum_i D_i B_{n,i}(t)
     now values of t for which P(t) lies outside of our fat line L
     corrsponds to values of t for which D(t) lies above d = dmax or
     below d = dmin
     we can determine parameter ranges of t for which P(t) is guaranteed
     to lie outside of L by identifying ranges of t which the convex hull
     of D(t) lies above dmax or below dmin
    */
    // now calculate the control points of D(t) by using the signed 
    // distances of P_i to our chord
    KoPathSegment dt;
    if( degree2 == 1 )
    {
        QPointF p0( 0.0, distanceFromChord( segment.first()->point() ) );
        QPointF p1( 1.0, distanceFromChord( segment.second()->point() ) );
        dt = KoPathSegment( p0, p1 );
    }
    else if( degree2 == 2 )
    {
        QPointF p0( 0.0, distanceFromChord( segment.first()->point() ) );
        QPointF p1 = segment.first()->activeControlPoint2() 
                   ? QPointF( 0.5, distanceFromChord( segment.first()->controlPoint2() ) )
                   : QPointF( 0.5, distanceFromChord( segment.second()->controlPoint1() ) );
        QPointF p2( 1.0, distanceFromChord( segment.second()->point() ) );
        dt = KoPathSegment( p0, p1, p2 );
    }
    else if( degree2 == 3 )
    {
        QPointF p0( 0.0, distanceFromChord( segment.first()->point() ) );
        QPointF p1( 1./3., distanceFromChord( segment.first()->controlPoint2() ) );
        QPointF p2( 2./3., distanceFromChord( segment.second()->controlPoint1() ) );
        QPointF p3( 1.0, distanceFromChord( segment.second()->point() ) );
        dt = KoPathSegment( p0, p1, p2, p3 );
    }
    else
    {
        //kDebug(30006) << "invalid degree of segment -> exiting";
        return isects;
    }

    // get convex hull of the segment D(t)
    QList<QPointF> hull = dt.convexHull();

    // now calculate intersections with the line y1 = dmin, y2 = dmax
    // with the convex hull edges
    int hullCount = hull.count();
    qreal tmin = 1.0, tmax = 0.0;
    bool intersectionsFoundMax = false;
    bool intersectionsFoundMin = false;

    for( int i = 0; i < hullCount; ++i )
    {
        QPointF p1 = hull[i];
        QPointF p2 = hull[(i+1)%hullCount];
        //kDebug(30006) << "intersecting hull edge (" << p1 << p2 << ")";
        // hull edge is completely above dmax
        if( p1.y() > dmax && p2.y() > dmax )
            continue;
        // hull egde is completely below dmin
        if( p1.y() < dmin && p2.y() < dmin )
            continue;
        if( p1.x() == p2.x() )
        {
            // vertical edge
            bool dmaxIntersection = ( dmax < qMax(p1.y(), p2.y()) && dmax > qMin(p1.y(),p2.y()) );
            bool dminIntersection = ( dmin < qMax(p1.y(), p2.y()) && dmin > qMin(p1.y(),p2.y()) );
            if( dmaxIntersection || dminIntersection )
            {
                tmin = qMin( tmin, p1.x() );
                tmax = qMax( tmax, p1.x() );
                if( dmaxIntersection )
                {
                    intersectionsFoundMax = true;
                    //kDebug(30006) << "found intersection with dmax at " << p1.x() << "," << dmax;
                }
                else
                {
                    intersectionsFoundMin = true;
                    //kDebug(30006) << "found intersection with dmin at " << p1.x() << "," << dmin;
                }
            }
        }
        else if( p1.y() == p2.y() )
        {
            // horizontal line
            if( p1.y() == dmin || p1.y() == dmax )
            {
                if( p1.y() == dmin )
                {
                    intersectionsFoundMin = true;
                    //kDebug(30006) << "found intersection with dmin at " << p1.x() << "," << dmin;
                    //kDebug(30006) << "found intersection with dmin at " << p2.x() << "," << dmin;
                }
                else
                {
                    intersectionsFoundMax = true;
                    //kDebug(30006) << "found intersection with dmax at " << p1.x() << "," << dmax;
                    //kDebug(30006) << "found intersection with dmax at " << p2.x() << "," << dmax;
                }
                tmin = qMin( tmin, p1.x() );
                tmin = qMin( tmin, p2.x() );
                tmax = qMax( tmax, p1.x() );
                tmax = qMax( tmax, p2.x() );
            }
        }
        else
        {
            qreal dx = p2.x()-p1.x();
            qreal dy = p2.y()-p1.y();
            qreal m = dy / dx;
            qreal n = p1.y() - m * p1.x();
            qreal t1 = (dmax - n) / m;
            if( t1 >= 0.0 && t1 <= 1.0 )
            {
                tmin = qMin( tmin, t1 );
                tmax = qMax( tmax, t1 );
                intersectionsFoundMax = true;
                //kDebug(30006) << "found intersection with dmax at " << t1 << "," << dmax;
            }
            qreal t2 = (dmin - n) / m;
            if( t2 >= 0.0 && t2 < 1.0 )
            {
                tmin = qMin( tmin, t2 );
                tmax = qMax( tmax, t2 );
                intersectionsFoundMin = true;
                //kDebug(30006) << "found intersection with dmin at " << t2 << "," << dmin;
            }
        }
    }

    bool intersectionsFound = intersectionsFoundMin && intersectionsFoundMax;

    //if( intersectionsFound )
    //    kDebug(30006) << "clipping segment to interval [" << tmin << "," << tmax << "]";

    if( ! intersectionsFound || (1.0-(tmax-tmin)) <= 0.2 )
    {
        //kDebug(30006) << "could not clip enough -> split segment";
        // we could not reduce the interval significantly
        // so split the curve and calculate intersections
        // with the remaining parts
        QPair<KoPathSegment,KoPathSegment> parts = splitAt( 0.5 );
        isects += segment.intersections( parts.first );
        isects += segment.intersections( parts.second );
    }
    else if( qAbs(tmin - tmax) < 1e-5 )
    {
        //kDebug(30006) << "Yay, we found an intersection";
        // the inteval is pretty small now, just calculate the intersection at this point
        isects.append( segment.pointAt( tmin ) );
    }
    else
    {
        QPair<KoPathSegment,KoPathSegment> clip1 = segment.splitAt( tmin );
        //kDebug(30006) << "splitting segment at" << tmin;
        qreal t = (tmax-tmin) / (1.0 - tmin);
        QPair<KoPathSegment,KoPathSegment> clip2 = clip1.second.splitAt( t );
        //kDebug(30006) << "splitting second part at" << t << "("<<tmax<<")";
        isects += clip2.first.intersections( *this );
    }

    return isects;
}

KoPathSegment KoPathSegment::mapped( const QMatrix &matrix ) const
{
    if( ! isValid() )
        return *this;

    KoPathPoint * p1 = new KoPathPoint( *d->first );
    KoPathPoint * p2 = new KoPathPoint( *d->second );
    p1->map( matrix );
    p2->map( matrix );

    return KoPathSegment( p1, p2 );
}

qreal KoPathSegment::length( qreal error ) const
{
    /*
     * This algorithm is implemented based on an idea by Jens Gravesen:
     * "Adaptive subdivision and the length of Bezier curves" mat-report no. 1992-10, Mathematical Institute,
     * The Technical University of Denmark.
     *
     * By subdividing the curve at parameter value t you only have to find the length of a full Bezier curve.
     * If you denote the length of the control polygon by L1 i.e.:
     *   L1 = |P0 P1| +|P1 P2| +|P2 P3|
     *
     * and the length of the cord by L0 i.e.:
     *   L0 = |P0 P3|
     *
     * then
     *   L = 1/2*L0 + 1/2*L1
     *
     * is a good approximation to the length of the curve, and the difference 
     *   ERR = L1-L0
     *
     * is a measure of the error. If the error is to large, then you just subdivide curve at parameter value
     * 1/2, and find the length of each half.
     * If m is the number of subdivisions then the error goes to zero as 2^-4m.
     * If you dont have a cubic curve but a curve of degree n then you put
     *   L = (2*L0 + (n-1)*L1)/(n+1)
     */

    int deg = degree();

    if( deg == -1 )
        return 0.0;

    QList<QPointF> ctrlPoints = controlPoints();

    // calculate chord length
    double chordLen = chordLength();

    if( deg == 1 )
    {
        return chordLen;
    }

    // calculate length of control polygon
    double polyLength = 0.0;

    for( int i = 0; i < deg; ++i )
    {
        QPointF ctrlSegment = ctrlPoints[i+1]-ctrlPoints[i];
        polyLength += sqrt( ctrlSegment.x()*ctrlSegment.x() + ctrlSegment.y()*ctrlSegment.y() );
    }

    if( (polyLength-chordLen) > error )
    {
        // the error is still bigger than our tolerance -> split segment
        QPair<KoPathSegment,KoPathSegment> parts = splitAt( 0.5 );
        return parts.first.length( error ) + parts.second.length( error );
    }
    else
    {
        // the error is smaller than our tolerance
        if( deg == 3 )
            return 0.5 * chordLen + 0.5 * polyLength;
        else
            return (2.0 * chordLen + polyLength) / 3.0;
    }
}

qreal KoPathSegment::lengthAt( qreal t, qreal error ) const
{
    if( t == 0.0 )
        return 0.0;
    if( t == 1.0 )
        return length( error );

    QPair<KoPathSegment,KoPathSegment> parts = splitAt( t );
    return parts.first.length( error );
}

qreal KoPathSegment::paramAtLength( qreal length, qreal tolerance ) const
{
    int deg = degree();
    if( deg < 1 )
        return 0.0;
    if( length <= 0.0 )
        return 0.0;

    if( deg == 1 )
        return length / chordLength();

    qreal startT = 0.0; // interval start
    qreal midT = 0.5;   // interval center
    qreal endT = 1.0;   // interval end

    qreal midLength = lengthAt( 0.5 );
    while( qAbs( midLength - length ) / length > tolerance )
    {
        if( midLength < length )
            startT = midT;
        else
            endT = midT;

        // new interval center
        midT = 0.5 * ( startT + endT );
        // length at new interval center
        midLength = lengthAt( midT );
    }

    return midT;
}

bool KoPathSegment::isFlat( qreal tolerance ) const
{
    /*
     * Calculate the height of the bezier curve.
     * This is done by rotating the curve so that then chord
     * is parallel to the x-axis and the calculating the 
     * parameters t for the extrema of the curve. 
     * The curve points at the extrema are then used to 
     * calculate the height.
     */
    if( degree() <= 1 )
        return true;

    QPointF chord = d->second->point() - d->first->point();
    // calculate angle of chord to the x-axis
    double chordAngle = atan2( chord.y(), chord.x() );
    QMatrix m;
    m.translate( d->first->point().x(), d->first->point().y() );
    m.rotate( chordAngle * M_PI / 180.0 );
    m.translate( -d->first->point().x(), -d->first->point().y() );

    KoPathSegment s = mapped( m );

    qreal minDist = 0.0;
    qreal maxDist = 0.0;

    foreach( qreal t, s.extrema() )
    {
        if( t >= 0.0 && t <= 1.0 )
        {
            QPointF p = pointAt( t );
            qreal dist = s.distanceFromChord( p );
            minDist = qMin( dist, minDist );
            maxDist = qMax( dist, maxDist );
        }
    }

    return ( maxDist - minDist <= tolerance );
}

qreal KoPathSegment::distanceFromChord( const QPointF &point ) const
{
    // the segments chord
    QPointF chord = d->second->point() - d->first->point();
    // the point relative to the segment
    QPointF relPoint = point - d->first->point();
    // project point to chord
    qreal scale = chord.x()*relPoint.x() + chord.y()*relPoint.y();
    scale /= chord.x()*chord.x()+chord.y()*chord.y();

    // the vector form the point to the projected point on the chord
    QPointF diffVec = scale * chord - relPoint;

    // the unsigned distance of the point to the chord
    qreal distance = sqrt( diffVec.x()*diffVec.x() + diffVec.y() * diffVec.y() );

    // determine sign of the distance using the cross product
    if( chord.x()*relPoint.y() - chord.y()*relPoint.x() > 0 )
    {
        return distance;
    }
    else
    {
        return -distance;
    }
}

qreal KoPathSegment::chordLength() const
{
    QPointF chord = d->second->point() - d->first->point();
    return sqrt( chord.x()*chord.x() + chord.y()*chord.y() );
}

QList<QPointF> KoPathSegment::convexHull() const
{
    QList<QPointF> hull;
    int deg = degree();
    if( deg == 1 )
    {
        // easy just the two end points
        hull.append( d->first->point() );
        hull.append( d->second->point() );
    }
    else if( deg == 2 )
    {
        // we want to have a counter-clockwise oriented triangle
        // of the three control points
        QPointF chord = d->second->point() - d->first->point();
        QPointF cp = d->first->activeControlPoint2() ? d->first->controlPoint2() : d->second->controlPoint1();
        QPointF relP = cp - d->first->point();
        // check on which side of the chord the control point is
        bool pIsRight = (chord.x()*relP.y() - chord.y()*relP.x() > 0 );
        hull.append( d->first->point() );
        if( pIsRight )
            hull.append( cp );
        hull.append( d->second->point() );
        if( ! pIsRight )
            hull.append( cp );
    }
    else if( deg == 3 )
    {
        // we want a counter-clockwise oriented polygon
        QPointF chord = d->second->point() - d->first->point();
        QPointF relP1 = d->first->controlPoint2() - d->first->point();
        // check on which side of the chord the control points are
        bool p1IsRight = (chord.x()*relP1.y() - chord.y()*relP1.x() > 0 );
        hull.append( d->first->point() );
        if( p1IsRight )
            hull.append( d->first->controlPoint2() );
        hull.append( d->second->point() );
        if( ! p1IsRight )
            hull.append( d->first->controlPoint2() );

        // now we have a counter-clockwise triangle with the points i,j,k
        // we have to check where the last control points lies
        bool rightOfEdge[3];
        QPointF lastPoint = d->second->controlPoint1();
        for( int i = 0; i < 3; ++i )
        {
            QPointF relP = lastPoint - hull[i];
            QPointF edge = hull[(i+1)%3] - hull[i];
            rightOfEdge[i] = (edge.x()*relP.y() - edge.y()*relP.x() > 0 );
        }
        for( int i = 0; i < 3; ++i )
        {
            int prev = (3+i-1)%3;
            int next = (i+1)%3;
            // check if point is only right of the n-th edge
            if( ! rightOfEdge[prev] && rightOfEdge[i] && ! rightOfEdge[next] )
            {
                // insert by breaking the n-th edge
                hull.insert( i+1, lastPoint );
                break;
            }
            // check if it is right of the n-th and right of the (n+1)-th edge
            if( rightOfEdge[i] && rightOfEdge[next] )
            {
                // remove both edge, insert two new edges
                hull[i+1] = lastPoint;
                break;
            }
            // check if it is right of n-th and right of (n-1)-th edge
            if( rightOfEdge[i] && rightOfEdge[prev] )
            {
                hull[i] = lastPoint;
                break;
            }
        }
    }

    return hull;
}

QPair<KoPathSegment, KoPathSegment> KoPathSegment::splitAt( qreal t ) const
{
    QPair<KoPathSegment,KoPathSegment> results;
    if( ! isValid() )
        return results;

    if( degree() == 1 )
    {
        QPointF p = d->first->point() + t * ( d->second->point() - d->first->point());
        results.first = KoPathSegment( d->first->point(), p );
        results.second = KoPathSegment( p, d->second->point() );
    }
    else
    {
        QPointF newCP2, newCP1, splitP, splitCP1, splitCP2;

        deCasteljau( t, &newCP2, &splitCP1, &splitP, &splitCP2, &newCP1 );

        if( degree() == 2 )
        {
            results.first = KoPathSegment( d->first->point(), splitCP1, splitP );
            results.second = KoPathSegment( splitP, splitCP2, d->second->point() );
        }
        else
        {
            results.first = KoPathSegment( d->first->point(), newCP2, splitCP1, splitP );
            results.second = KoPathSegment( splitP, splitCP2, newCP1, d->second->point() );
        }
    }

    return results;
}

void KoPathSegment::deCasteljau( qreal t, QPointF *p1, QPointF *p2, QPointF *p3, QPointF *p4, QPointF *p5 ) const
{
    int deg = degree();
    QPointF q[4];

    q[0] = d->first->point();
    if( deg == 2 )
    {
        q[1] = d->first->activeControlPoint2() ? d->first->controlPoint2() : d->second->controlPoint1();
    }
    else if( deg == 3 )
    {
        q[1] = d->first->controlPoint2();
        q[2] = d->second->controlPoint1();
    }
    q[deg] = d->second->point();

    // points of the new segment after the split point
    QPointF p[3];

    // the De Casteljau algorithm
    for( unsigned short j = 1; j <= deg; ++j )
    {
        for( unsigned short i = 0; i <= deg - j; ++i )
        {
            q[i] = ( 1.0 - t ) * q[i] + t * q[i + 1];
        }
        p[j - 1] = q[0];
    }

    if( deg == 2 )
    {
        if( p2 )
            *p2 = p[0];
        if( p3 )
            *p3 = p[1];
        if( p4 )
            *p4 = q[1];
    }
    else if( deg == 3 )
    {
        if( p1 )
            *p1 = p[0];
        if( p2 )
            *p2 = p[1];
        if( p3 )
            *p3 = p[2];
        if( p4 )
            *p4 = q[1];
        if( p5 )
            *p5 = q[2];
    }
}

QList<QPointF> KoPathSegment::controlPoints() const
{
    QList<QPointF> controlPoints;
    controlPoints.append( d->first->point() );
    if( d->first->activeControlPoint2() )
        controlPoints.append( d->first->controlPoint2() );
    if( d->second->activeControlPoint1() )
        controlPoints.append( d->second->controlPoint1() );
    controlPoints.append( d->second->point() );

    return controlPoints;
}

QList<QPointF> KoPathSegment::linesIntersection( const KoPathSegment &segment ) const
{
    //kDebug(30006) << "intersecting two lines";
    /*
    we have to line segments:

    s1 = A + r * (B-A), s2 = C + s * (D-C) for r,s in [0,1]

        if s1 and s2 intersect we set s1 = s2 so we get these two equations:

    Ax + r * (Bx-Ax) = Cx + s * (Dx-Cx)
    Ay + r * (By-Ay) = Cy + s * (Dy-Cy)

    which we can solve to get r and s
    */
    QList<QPointF> isects;
    QPointF A = d->first->point();
    QPointF B = d->second->point();
    QPointF C = segment.first()->point();
    QPointF D = segment.second()->point();

    qreal denom = (B.x()-A.x())*(D.y()-C.y()) - (B.y()-A.y())*(D.x()-C.x());
    qreal num_r = (A.y()-C.y())*(D.x()-C.x())-(A.x()-C.x())*(D.y()-C.y());
        // check if lines are collinear
    if( denom == 0.0 && num_r == 0.0 )
        return isects;

    qreal num_s = (A.y()-C.y())*(B.x()-A.x())-(A.x()-C.x())*(B.y()-A.y());
    qreal r = num_r / denom;
    qreal s = num_s / denom;

        // check if intersection is inside our line segments
    if( r < 0.0 || r > 1.0 )
        return isects;
    if( s < 0.0 || s > 1.0 )
        return isects;

        // calculate the actual intersection point
    isects.append( A + r * (B-A) );

    return isects;
}

QList<qreal> KoPathSegment::extrema() const
{
    int deg = degree();
    if( deg <= 1 )
        return QList<qreal>();

    QList<qreal> params;

    /*
     * The basic idea for calculating the extrama for bezier segments
     * was found in comp.graphics.algorithms:
     *
     * Both the x coordinate and the y coordinate are polynomial. Newton told
     * us that at a maximum or minimum the derivative will be zero.
     *
     * We have a helpful trick for the derivatives: use the curve r(t) defined by
     * differences of successive control points.
     * Setting r(t) to zero and using the x and y coordinates of differences of
     * successive control points lets us find the parameters t, where the original
     * bezier curve has a minimum or a maximum.
     */
    if( deg == 2 )
    {
        /*
         * For quadratic bezier curves r(t) is a linear Bezier curve:
         *
         *  1
         * r(t) = Sum Bi,1(t) *Pi = B0,1(t) * P0 + B1,1(t) * P1
         * i=0
         *
         * r(t) = (1-t) * P0 + t * P1
         *
         * r(t) = (P1 - P0) * t + P0
         */

        // calcualting the differences between successive control points
        QPointF cp = d->first->activeControlPoint2() ? 
                d->first->controlPoint2() : d->second->controlPoint1();
        QPointF x0 = cp - d->first->point();
        QPointF x1 = d->second->point() - cp;

        // calculating the coefficents
        QPointF a = x1 - x0;
        QPointF c = x0;

        if( a.x() != 0.0 )
            params.append( -c.x() / a.x() );
        if( a.y() != 0.0 )
            params.append( -c.y() / a.y() );
    }
    else if( deg == 3 )
    {
        /*
         * For cubic bezier curves r(t) is a quadratic Bezier curve:
         *
         *  2
         * r(t) = Sum Bi,2(t) *Pi = B0,2(t) * P0 + B1,2(t) * P1 + B2,2(t) * P2
         * i=0
         *
         * r(t) = (1-t)^2 * P0 + 2t(1-t) * P1 + t^2 * P2
         *
         * r(t) = (P2 - 2*P1 + P0) * t^2 + (2*P1 - 2*P0) * t + P0
         *
         */
        // calcualting the differences between successive control points
        QPointF x0 = d->first->controlPoint2() - d->first->point();
        QPointF x1 = d->second->controlPoint1() - d->first->controlPoint2();
        QPointF x2 = d->second->point() - d->second->controlPoint1();

        // calculating the coefficents
        QPointF a = x2 - 2.0*x1 + x0;
        QPointF b = 2.0*x1 - 2.0*x0;
        QPointF c = x0;

        // calculating parameter t at minimum/maximum in x-direction
        if( a.x() == 0.0 )
        {
            params.append( - c.x() / b.x() );
        }
        else
        {
            double rx = b.x()*b.x() - 4.0*a.x()*c.x();
            if( rx < 0.0 )
                rx = 0.0;
            params.append( ( -b.x() + sqrt( rx ) ) / ( 2.0*a.x() ) );
            params.append( ( -b.x() - sqrt( rx ) ) / ( 2.0*a.x() ) );
        }

        // calculating parameter t at minimum/maximum in y-direction
        if( a.y() == 0.0 )
        {
            params.append( - c.y() / b.y() );
        }
        else
        {
            double ry = b.y()*b.y() - 4.0*a.y()*c.y();
            if( ry < 0.0 )
                ry = 0.0;
            params.append( ( -b.y() + sqrt( ry ) ) / ( 2.0*a.y() ) );
            params.append( ( -b.y() - sqrt( ry ) ) / ( 2.0*a.y() ) );
        }
    }

    return params;
}

void KoPathSegment::printDebug() const
{
    int deg = degree();
    kDebug(30006) << "degree:" << deg;
    if( deg < 1 )
        return;

    kDebug(30006) << "P0:" << d->first->point();
    if( deg == 2 )
    {
        if( d->first->activeControlPoint2() )
            kDebug(30006) << "P1:" << d->first->controlPoint2();
        else
            kDebug(30006) << "P1:" << d->second->controlPoint1();
        kDebug(30006) << "P2:" << d->second->point();
    }
    else if( deg == 3 )
    {
        kDebug(30006) << "P1:" << d->first->controlPoint2();
        kDebug(30006) << "P2:" << d->second->controlPoint1();
        kDebug(30006) << "P3:" << d->second->point();
    }
}
