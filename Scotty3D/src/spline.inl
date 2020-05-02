// Given a time between 0 and 1, evaluates a cubic polynomial with
// the given endpoint and tangent values at the beginning (0) and
// end (1) of the interval.  Optionally, one can request a derivative
// of the spline (0=no derivative, 1=first derivative, 2=2nd derivative).
using namespace std;

template <class T>
inline T Spline<T>::cubicSplineUnitInterval(
    const T& position0, const T& position1, const T& tangent0,
    const T& tangent1, double normalizedTime, int derivative) {
  // (Animation) Task 1a

  double t1 = normalizedTime;
  double t2 = t1 * t1;
  double t3 = t1 * t1 * t1;

  if (derivative == 0) {

    double h00 = 2 * t3 - 3 * t2 + 1;
    double h10 = t3 - 2 * t2 + t1;
    double h01 = -2 * t3 + 3 * t2;
    double h11 = t3 - t2;
    return h00 * position0 + h10 * tangent0 + h01 * position1 + h11 * tangent1;

  } else if (derivative == 1) {

    double h00 = 6 * t2 - 6 * t1;
    double h10 = 3 * t2 - 4 * t1 + 1;
    double h01 = -6 * t2 + 6 * t1;
    double h11 = 3 * t2 - 2 * t1;
    return h00 * position0 + h10 * tangent0 + h01 * position1 + h11 * tangent1;

  } else if (derivative == 2) {

    double h00 = 12 * normalizedTime - 6;
    double h10 = 6 * normalizedTime - 4;
    double h01 = -12 * normalizedTime + 6;
    double h11 = 6 * normalizedTime - 2;
    return h00 * position0 + h10 * tangent0 + h01 * position1 + h11 * tangent1;

  } else {
    return T();
  }

}

// Returns a state interpolated between the values directly before and after the
// given time.
template <class T>
inline T Spline<T>::evaluate(double time, int derivative) {
  // (Animation) Task 1b
  double t_norm;
  T p0, p1, m0, m1;

  if (knots.size() < 1) return T();

  if (knots.size() == 1) return derivative == 0 ? knots.begin()->second : T();

  if (time <= knots.begin()->first) return derivative == 0 ? knots.begin()->second : T();

  if (time >= knots.rbegin()->first) return derivative == 0 ? knots.rbegin()->second : T();

  KnotIter kn2 = knots.upper_bound(time);
  KnotIter kn1 = prev(knots.upper_bound(time));
  KnotIter kn0, kn3;

  T k0, k1, k2, k3;
  double t0, t1, t2, t3;

  k1 = kn1->second;
  k2 = kn2->second;

  t1 = kn1->first;
  t2 = kn2->first;

  if (kn1 == knots.begin()) {
    k0 = k1 - (k2 - k1);
    t0 = t1 - (t2 - t1);
  } else {
    kn0 = prev(kn1);
    k0 = kn0->second;
    t0 = kn0->first;
  }

  if (kn2 == prev(knots.end())) {
    k3 = k2 + (k2 - k1);
    t3 = t2 + (t2 - t1);
  } else {
    kn3 = next(kn2);
    k3 = kn3->second;
    t3 = kn3->first;
  }

  p0 = k1;
  p1 = k2;
  m0 = (k2 - k0) * (t2 - t1) / (t2 - t0);
  m1 = (k3 - k1) * (t2 - t1) / (t3 - t1);
  t_norm = (time - t1) / (t2 - t1);

  return cubicSplineUnitInterval(p0, p1, m0, m1, t_norm, derivative);
}

// Removes the knot closest to the given time,
//    within the given tolerance..
// returns true iff a knot was removed.
template <class T>
inline bool Spline<T>::removeKnot(double time, double tolerance) {
  // Empty maps have no knots.
  if (knots.size() < 1) {
    return false;
  }

  // Look up the first element > or = to time.
  typename std::map<double, T>::iterator t2_iter = knots.lower_bound(time);
  typename std::map<double, T>::iterator t1_iter;
  t1_iter = t2_iter;
  t1_iter--;

  if (t2_iter == knots.end()) {
    t2_iter = t1_iter;
  }

  // Handle tolerance bounds,
  // because we are working with floating point numbers.
  double t1 = (*t1_iter).first;
  double t2 = (*t2_iter).first;

  double d1 = fabs(t1 - time);
  double d2 = fabs(t2 - time);

  if (d1 < tolerance && d1 < d2) {
    knots.erase(t1_iter);
    return true;
  }

  if (d2 < tolerance && d2 < d1) {
    knots.erase(t2_iter);
    return t2;
  }

  return false;
}

// Sets the value of the spline at a given time (i.e., knot),
// creating a new knot at this time if necessary.
template <class T>
inline void Spline<T>::setValue(double time, T value) {
  knots[time] = value;
}

template <class T>
inline T Spline<T>::operator()(double time) {
  return evaluate(time);
}
