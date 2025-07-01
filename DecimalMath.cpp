// DecimalMath.cpp
#include "DecimalMath.h"

// Constructor from string
Decimal::Decimal(const String& str) {
  int dotPos = str.indexOf('.');
  
  if (dotPos == -1) {
    // No decimal point - it's an integer
    value = str.toInt() * SCALE;
  } else {
    // Has decimal point
    String wholePart = str.substring(0, dotPos);
    String fracPart = str.substring(dotPos + 1);
    
    // Pad or truncate fractional part to 6 digits
    while (fracPart.length() < 6) fracPart += "0";
    if (fracPart.length() > 6) fracPart = fracPart.substring(0, 6);
    
    long wholeValue = wholePart.toInt();
    long fracValue = fracPart.toInt();
    
    // Handle negative numbers correctly
    if (str.startsWith("-") && wholeValue == 0) {
      value = -fracValue;
    } else if (wholeValue < 0) {
      value = wholeValue * SCALE - fracValue;
    } else {
      value = wholeValue * SCALE + fracValue;
    }
  }
}

// Arithmetic operators
Decimal Decimal::operator+(const Decimal& other) const {
  return Decimal::fromScaled(value + other.value);
}

Decimal Decimal::operator-(const Decimal& other) const {
  return Decimal::fromScaled(value - other.value);
}

Decimal Decimal::operator*(const Decimal& other) const {
  // Use long long to prevent overflow during multiplication
  long long result = ((long long)value * other.value) / SCALE;
  return Decimal::fromScaled((long)result);
}

Decimal Decimal::operator/(const Decimal& other) const {
  if (other.value == 0) {
    return Decimal::fromScaled(0); // Division by zero returns 0
  }
  
  // Use long long to prevent overflow during division
  long long result = ((long long)value * SCALE) / other.value;
  return Decimal::fromScaled((long)result);
}

Decimal Decimal::operator-() const {
  return Decimal::fromScaled(-value);
}

// String conversion
String Decimal::toString() const {
  if (value == 0) return "0";
  
  long absValue = value >= 0 ? value : -value;
  long wholePart = absValue / SCALE;
  long fracPart = absValue % SCALE;
  
  String result = "";
  if (value < 0) result += "-";
  result += String(wholePart);
  
  if (fracPart != 0) {
    result += ".";
    
    // Format fractional part with leading zeros if needed
    String fracStr = String(fracPart);
    while (fracStr.length() < 6) fracStr = "0" + fracStr;
    
    // Remove trailing zeros
    while (fracStr.endsWith("0")) {
      fracStr.remove(fracStr.length() - 1);
    }
    
    result += fracStr;
  }
  
  return result;
}

// Helper function for debugging
void printDecimalDebug(const Decimal& d) {
  Serial.print("Decimal(");
  Serial.print(d.value);
  Serial.print(" -> ");
  Serial.print(d.toString());
  Serial.println(")");
}

// Parse a simple decimal value from string
Decimal parseDecimalValue(const String& token) {
  return Dec(token);
}

// Parse mixed fraction and convert to decimal
Decimal parseMixedFractionDecimal(const String& token) {
  int spacePos = token.indexOf(' ');
  
  if (spacePos >= 0) {
    // Mixed number like "1 1/2"
    Decimal wholePart = Dec(token.substring(0, spacePos));
    String fracPart = token.substring(spacePos + 1);
    
    int slashPos = fracPart.indexOf('/');
    if (slashPos >= 0) {
      Decimal numerator = Dec(fracPart.substring(0, slashPos));
      Decimal denominator = Dec(fracPart.substring(slashPos + 1));
      
      if (!denominator.isZero()) {
        Decimal fracValue = numerator / denominator;
        return wholePart + (wholePart >= Dec(0L) ? fracValue : -fracValue);
      }
    }
    return wholePart;
  }
  
  // Check if it's a simple fraction
  int slashPos = token.indexOf('/');
  if (slashPos >= 0) {
    Decimal numerator = Dec(token.substring(0, slashPos));
    Decimal denominator = Dec(token.substring(slashPos + 1));
    
    if (!denominator.isZero()) {
      return numerator / denominator;
    }
    return numerator;
  }
  
  // Regular decimal number
  return Dec(token);
}

// Evaluate expression using decimal arithmetic
Decimal evaluateDecimalExpression(const String& expr) {
  String s = expr;
  s.replace("x", "*");
  
  Decimal result = Dec(0L);
  char op = '+';
  int start = 0;
  
  for (int i = 0; i <= s.length(); i++) {
    char c = (i < s.length()) ? s[i] : '\0';
    bool isOp = (c == '+' || c == '-' || c == '*' || c == '/' || c == '\0');
    
    if (isOp) {
      if (i > start) {
        String token = s.substring(start, i);
        Decimal val = parseMixedFractionDecimal(token);
        
        switch (op) {
          case '+': result = result + val; break;
          case '-': result = result - val; break;
          case '*': result = result * val; break;
          case '/': result = result / val; break;
        }
      }
      op = c;
      start = i + 1;
    }
  }
  
  return result;
}