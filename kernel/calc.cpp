/*
 * calc.cpp
 *
 *  Created on: 2009/10/30
 *      Author: uchan
 */

#include <string.h>
#include <stdlib.h>

#include <bitnos/calc.h>

int pow(int x, unsigned int p)
{
    int result = 1;
    int temp = x;
    while (p > 0) {
        if ((p & 1) == 1) {
            result *= temp;
        }
        temp = temp * temp;
        p = (p >> 1);
    }
    return result;
}

namespace Calc {

    bool IsSpace(char c)
    {
        return c == ' ' || c == '\t';
    }

    bool IsDigit(char c)
    {
        return '0' <= c && c <= '9';
    }

    bool IsSingleOperator(char c)
    {
        return c == '+' || c == '-' || c == '~';
    }

    const char* SkipSpace(const char* p)
    {
        while (*p != '\0' && IsSpace(*p)) {
            p++;
        }
        return p;
    }

    class Stack
    {
        static const int stackSize = 32;
        int stack[stackSize];
        int sp;
        int dummy;

        public:
        void Init();

        void Push(int item);

        int Pop();

        int& operator[](int index);
    };

    void Stack::Init()
    {
        sp = stackSize - 1;
        dummy = 123454321;
    }

    void Stack::Push(int item)
    {
        if (sp >= 0) {
            stack[sp--] = item;
        }
    }

    int Stack::Pop()
    {
        if (sp < stackSize - 1) {
            return stack[++sp];
        }
        return 0;
    }

    int& Stack::operator [](int index)
    {
        if (0<= index && index < stackSize) {
            return stack[index];
        }
        return dummy;
    }

    int RPNCalc(const char* exp)
    {
        Stack stack;
        char elem[32];
        int i = 0;
        int var1, var2;

        char sop;

        stack.Init();

        while (*exp != '\0') {
            exp = SkipSpace(exp);
            i = 0;
            sop = '\0';
            if (IsSingleOperator(exp[0]) && IsDigit(exp[1])) {
                sop = *exp++;
                while (*exp != '\0' && IsDigit(*exp)) {
                    elem[i++] = *exp++;
                }
            } else if (IsDigit(*exp)) {
                while (*exp != '\0' && IsDigit(*exp)) {
                    elem[i++] = *exp++;
                }
            } else {
                elem[i++] = *exp++;
            }
            elem[i] = '\0';

            if (strcmp("+", elem) == 0) {
                var2 = stack.Pop();
                var1 = stack.Pop();
                stack.Push(var1 + var2);
            } else if (strcmp("-", elem) == 0) {
                var2 = stack.Pop();
                var1 = stack.Pop();
                stack.Push(var1 - var2);
            } else if (strcmp("*", elem) == 0) {
                var2 = stack.Pop();
                var1 = stack.Pop();
                stack.Push(var1 * var2);
            } else if (strcmp("/", elem) == 0) {
                var2 = stack.Pop();
                var1 = stack.Pop();
                if (var2 != 0) {
                    stack.Push(var1 / var2);
                } else {
                    stack.Push(0);
                }
            } else if (strcmp("%", elem) == 0) {
                var2 = stack.Pop();
                var1 = stack.Pop();
                stack.Push(var1 % var2);
            } else if (strcmp("^", elem) == 0) {
                var2 = stack.Pop();
                var1 = stack.Pop();
                stack.Push(pow(var1, var2));
            } else {
                var1 = strtol(elem, 0, 0);
                if (sop == '-') {
                    var1 = -var1;
                } else if (sop == '~') {
                    var1 = ~var1;
                }
                stack.Push(var1);
            }
        }

        return stack.Pop();
    }

    class Convert
    {
        char* s;
        const char* exp;
        void E();
        void T();
        void F();
        public:
        void Do(char* s, const char* exp);
    };

    void Convert::E()
    {
        T();
        while (!0) {
            const char* p = exp = SkipSpace(exp);
            if (*exp == '+' || *exp == '-') {
                char c = *exp++;
                T();
                *s++ = ' ';
                *s++ = c;
            } else {
                break;
            }
            if (p == exp) {
                break;
            }
        }
    }

    void Convert::T()
    {
        F();
        while (!0) {
            const char* p = exp = SkipSpace(exp);
            if (*exp == '*' || *exp == '/') {
                char c = *exp++;
                F();
                *s++ = ' ';
                *s++ = c;
            } else {
                break;
            }
            if (p == exp) {
                break;
            }
        }
    }

    void Convert::F()
    {
        exp = SkipSpace(exp);
        if (*exp == '(') {
            exp++;
            E();
            exp = SkipSpace(exp);
            if (*exp == ')') {
                exp++;
            }
        } else {
            *s++ = ' ';
            if (*exp == '+' || *exp == '-') {
                *s++ = *exp++;
            }
            while (*exp != '\0' && IsDigit(*exp)) {
                *s++ = *exp++;
            }

        }
    }

    void Convert::Do(char* s, const char* exp)
    {
        // 中置→後置
        this->s = s;
        this->exp = exp;
        E();
        *this->s = '\0';
        if (*this->exp != '\0') {
            strcat(s, " ERR");
        }

    }

    int Calc(const char* exp)
    {
        Convert conv;
        char s[128];
        conv.Do(s, exp);
        return RPNCalc(s);
    }

}
