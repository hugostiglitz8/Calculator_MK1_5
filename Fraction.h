//Fraction.h

#ifndef FRACTION_H
#define FRACTION_H

#include <Arduino.h>  // for String, abs(), lround()

struct Fraction {
  long numerator, denominator;

  // ctor
  Fraction(long n = 0, long d = 1);

  // methods
  void simplify();
  long gcd(long a, long b) const;
  double toDecimal() const;

  // these MUST match the definitions exactly:
  Fraction operator+(const Fraction& o) const;
  Fraction operator-(const Fraction& o) const;
  Fraction operator*(const Fraction& o) const;
  Fraction operator/(const Fraction& o) const;
};

// helpers
Fraction decimalToSixtyFourths(double v);
String  formatMixedFraction(const Fraction& f);

#endif // FRACTION_H