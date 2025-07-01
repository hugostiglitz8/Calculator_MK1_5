// DecimalMath.h
#ifndef DECIMAL_MATH_H
#define DECIMAL_MATH_H

#include <Arduino.h>

// Fixed-point decimal with 6 decimal places (1,000,000 scale)
// This gives us range of about ±2,000 with 6 decimal precision
class Decimal {
public:
  static const long SCALE = 1000000L; // 6 decimal places
  
private:
  long value; // Internal representation (scaled by SCALE)
  
public:
  // Constructors
  Decimal() : value(0) {}
  explicit Decimal(long v) : value(v * SCALE) {}
  explicit Decimal(double d) : value((long)(d * SCALE + (d >= 0 ? 0.5 : -0.5))) {}
  explicit Decimal(const String& str);
  
  // Static factory method for internal scaled values
  static Decimal fromScaled(long scaled) {
    Decimal d;
    d.value = scaled;
    return d;
  }
  
  // Arithmetic operators
  Decimal operator+(const Decimal& other) const;
  Decimal operator-(const Decimal& other) const;
  Decimal operator*(const Decimal& other) const;
  Decimal operator/(const Decimal& other) const;
  Decimal operator-() const; // Unary minus
  
  // Comparison operators
  bool operator==(const Decimal& other) const { return value == other.value; }
  bool operator!=(const Decimal& other) const { return value != other.value; }
  bool operator<(const Decimal& other) const { return value < other.value; }
  bool operator>(const Decimal& other) const { return value > other.value; }
  bool operator<=(const Decimal& other) const { return value <= other.value; }
  bool operator>=(const Decimal& other) const { return value >= other.value; }
  
  // Conversion methods
  double toDouble() const { return (double)value / SCALE; }
  long toInt() const { return value / SCALE; }
  String toString() const;
  
  // Utility methods
  bool isZero() const { return value == 0; }
  bool isInteger() const { return (value % SCALE) == 0; }
  Decimal absolute() const { return value >= 0 ? *this : Decimal::fromScaled(-value); }
  
  // Friend function for debugging
  friend void printDecimalDebug(const Decimal& d);
};

// Helper functions for parsing expressions
Decimal parseDecimalValue(const String& token);
Decimal parseMixedFractionDecimal(const String& token);
Decimal evaluateDecimalExpression(const String& expr);

// Helper function to create decimals easily
inline Decimal Dec(long v) { return Decimal(v); }
inline Decimal Dec(double v) { return Decimal(v); }
inline Decimal Dec(const String& s) { return Decimal(s); }

#endif // DECIMAL_MATH_H