//Fraction.cpp

#include "Fraction.h"

// ——— ctor & simplify ———
Fraction::Fraction(long n, long d)
  : numerator(n), denominator(d)
{
  if (denominator == 0) denominator = 1;
  simplify();
}

void Fraction::simplify() {
  if (denominator < 0) {
    numerator = -numerator;
    denominator = -denominator;
  }
  long g = gcd(abs(numerator), abs(denominator));
  numerator /= g;
  denominator /= g;
}

long Fraction::gcd(long a, long b) const {
  while (b != 0) {
    long t = b;
    b = a % b;
    a = t;
  }
  return a;
}

double Fraction::toDecimal() const {
  return (double)numerator / denominator;
}

// ——— operators ———
Fraction Fraction::operator+(const Fraction& o) const {
  return Fraction(numerator*o.denominator + o.numerator*denominator,
                  denominator*o.denominator);
}

Fraction Fraction::operator-(const Fraction& o) const {
  return Fraction(numerator*o.denominator - o.numerator*denominator,
                  denominator*o.denominator);
}

Fraction Fraction::operator*(const Fraction& o) const {
  return Fraction(numerator*o.numerator,
                  denominator*o.denominator);
}

Fraction Fraction::operator/(const Fraction& o) const {
  return Fraction(numerator*o.denominator,
                  denominator*o.numerator);
}

// ——— helpers ———
Fraction decimalToSixtyFourths(double v) {
  long n = lround(v * 64);
  return Fraction(n, 64);
}

String formatMixedFraction(const Fraction& f) {
  long n = abs(f.numerator), d = f.denominator;
  long w = n/d, r = n%d;
  String s = "";
  if (f.numerator < 0) s += "-";
  if (w)           s += String(w);
  if (r) {
    if (w) s += " ";
    s += String(r) + "/" + String(d);
  }
  if (!w && !r)    s = "0";
  return s;
}