//CalculatorLogic.ino
#include "Display.h"
#include "Keypad.h"
#include "DecimalMath.h"

// calculator state
String historyLine="", calcLine="", alternateDisplay="";
bool justCalculated=false, usedFractionEntry=false;
Fraction currentFraction(0,1), storedFraction(0,1);
enum Operation { OP_NONE, OP_ADD, OP_SUBTRACT, OP_MULTIPLY, OP_DIVIDE };
Operation currentOperation = OP_NONE;

// Answer memory for numberpad mode
String lastAnswer = "";
bool hasStoredAnswer = false;

void clearAll() {
  calcLine = "";
  historyLine = "";
  alternateDisplay = "";
  justCalculated = false;
  currentFraction = Fraction(0, 1);
  storedFraction = Fraction(0, 1);
  currentOperation = OP_NONE;
  usedFractionEntry = false;
  // Don't clear lastAnswer - keep it available for numberpad mode
  // Clear display area
  clearDisplayArea();
}

void handleCalculatorMode(const char* key) {
  // Quick aliases
  char first = key[0];

  // AC
  if (strcmp(key, "AC") == 0) {
    clearAll();
    return;
  }
  // Mixed-number space
  if (strcmp(key, ">") == 0) {
    if (calcLine.length() && calcLine.endsWith(" ") == false)
      calcLine += " ";
    return;
  }
  // x/y fraction
  if (strcmp(key, "x/y") == 0) {
    usedFractionEntry = true;
    if (calcLine.length() && calcLine.endsWith("/") == false && calcLine.endsWith(" ") == false)
      calcLine += "/";
    return;
  }
  // Delete
  if (strcmp(key, "Del") == 0) {
    if (!justCalculated && calcLine.length())
      calcLine.remove(calcLine.length() - 1);
    return;
  }

  // MM ↔ in modifier
  if (strcmp(key, "MM") == 0) {
    if (calcLine.length()) {
      Decimal val = evaluateDecimalExpression(calcLine);
      
      // Store the pure decimal value BEFORE adding units
      lastAnswer = val.toString();
      hasStoredAnswer = true;
      
      if (zeroHoldActive) {
        Decimal inches = val / Dec("25.4");
        calcLine = inches.toString() + " in";
        // Update stored answer to the converted value (without units)
        lastAnswer = inches.toString();
      } else {
        Decimal mm = val * Dec("25.4");
        calcLine = mm.toString() + " mm";
        // Update stored answer to the converted value (without units)
        lastAnswer = mm.toString();
      }
      justCalculated = true;
      zeroHoldActive = false;  // reset
      
      Serial.print("MM conversion - Answer stored for numberpad: ");
      Serial.println(lastAnswer);
    }
    return;
  }

  // round (toggle fraction/decimal)
  if (strcmp(key, "round") == 0) {
    if (calcLine.length()) {
      if (containsFraction(calcLine)) {
        Decimal v = evaluateDecimalExpression(calcLine);
        calcLine = v.toString();
        // Store the decimal version
        lastAnswer = v.toString();
        hasStoredAnswer = true;
      } else {
        Decimal v = evaluateDecimalExpression(calcLine);
        double doubleVal = v.toDouble();
        Fraction f = decimalToSixtyFourths(doubleVal);
        calcLine = formatMixedFraction(f);
        // Always store the decimal version, even when displaying fraction
        lastAnswer = v.toString();
        hasStoredAnswer = true;
      }
      justCalculated = true;
      
      Serial.print("Round conversion - Answer stored for numberpad: ");
      Serial.println(lastAnswer);
    }
    return;
  }

  // After a result, new operator or digit starts fresh
  if (justCalculated) {
    if (isOperator(key)) {
      calcLine += key;
      justCalculated = false;
      return;
    }
    if (isNumber(key)) {
      calcLine = String(key);
      justCalculated = false;
      usedFractionEntry = false;
      return;
    }
  }

  // Digit
  if (isNumber(key)) {
    calcLine += key;
    return;
  }
  // Operator
  if (isOperator(key)) {
    calcLine += key;
    return;
  }
  // Decimal point
  if (first == '.') {
    calcLine += key;
    return;
  }
  // Calculate
  if (strcmp(key, "return") == 0) {
    if (calcLine.length() && !justCalculated) {
      String expr = calcLine;
      Decimal ans = evaluateDecimalExpression(expr);
      historyLine = expr;

      // ALWAYS store the pure decimal answer for numberpad mode
      lastAnswer = ans.toString();
      hasStoredAnswer = true;

      if (usedFractionEntry) {
        double doubleAns = ans.toDouble();
        Fraction f = decimalToSixtyFourths(doubleAns);
        if (abs(doubleAns - f.toDecimal()) < 0.000001) {
          calcLine = formatMixedFraction(f);
          alternateDisplay = ans.toString();
        } else {
          calcLine = ans.toString();
          alternateDisplay = "";
        }
      } else {
        calcLine = ans.toString();
        double doubleAns = ans.toDouble();
        Fraction f = decimalToSixtyFourths(doubleAns);
        if (abs(doubleAns - f.toDecimal()) < 0.000001) {
          alternateDisplay = formatMixedFraction(f);
        } else {
          alternateDisplay = "";
        }
      }

      justCalculated = true;
      usedFractionEntry = false;
      
      Serial.print("Answer stored for numberpad: ");
      Serial.println(lastAnswer);
    }
    return;
  }
}

// Getter function for numberpad mode to access stored answer
String getStoredAnswer() {
  return hasStoredAnswer ? lastAnswer : "";
}

bool hasCalculatorAnswer() {
  return hasStoredAnswer;
}

// Clear stored answer (optional - for if you want to clear it after use)
void clearStoredAnswer() {
  hasStoredAnswer = false;
  lastAnswer = "";
}

// Legacy functions kept for fraction support
double evaluateExpression(const String& expr) {
  Decimal result = evaluateDecimalExpression(expr);
  return result.toDouble();
}

double parseMixedFractionValue(const String& token) {
  Decimal result = parseMixedFractionDecimal(token);
  return result.toDouble();
}

double parseFraction(const String& frac) {
  int slash = frac.indexOf('/');
  if (slash == -1) return frac.toFloat();
  
  Decimal n = Dec(frac.substring(0, slash));
  Decimal d = Dec(frac.substring(slash + 1));
  
  if (d.isZero()) return 0;
  return (n / d).toDouble();
}

bool containsFraction(const String& expr) {
  if (expr.indexOf(' ') >= 0 && expr.indexOf('/') >= 0) return true;
  return expr.indexOf('/') >= 0;
}

String formatAnswer(double v) {
  // This function is now deprecated in favor of Decimal.toString()
  // But kept for backward compatibility with fraction code
  Decimal d = Dec(v);
  return d.toString();
}

inline bool isNumber(const char* k) {
  return (k[0] >= '0' && k[0] <= '9');
}
inline bool isOperator(const char* k) {
  return (k[0] == '+' || k[0] == '-' || k[0] == '*' || k[0] == '/');
}