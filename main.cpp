#include <cstdio>
#include <cmath>
#include <cfloat>

int solve_lin_eq(double k, double b, double *x);
int solve_quad_eq(double a, double b, double c, double *x1, double *x2);

int main()
{
    double a = 0, b = 0, c = 0, x1 = 0, x2 = 0;

    printf("Введите коэффициенты уравнения ax^2 + bx + c = 0\n");
    printf("a = ");
    scanf("%lf", &a);
    printf("b = ");
    scanf("%lf", &b);
    printf("c = ");
    scanf("%lf", &c);

    switch (solve_quad_eq(a, b, c, &x1, &x2))
    {
        case 2:
            printf("Найдено 2 решения: %.3e и %.3e\n", x1, x2);
            break;
        case 1:
            printf("Найдено одно решение: %.3e\n", x1);
            break;
        case 0:
            printf("Решений не найдено\n");
            break;
        case -1:
            printf("Решений бесконечно много\n");
            break;
        default:
            fprintf(stderr, "Неожиданное количество корней");
            return -1;
    }

    return 0;
}

/**
 * Решает квадратное уравнение вида a*x^2 + b*x + c = 0, записывает решения (при наличии) в переданные переменные (x1 и x2)
 * и возвращает их количество или -1, если их бесконечно много. \n\n
 *
 * Если решений меньше двух, неиспользованные переменные не изменяют своего значения.
 * При наличии одного решения оно записывается в x1.
 * При наличии двух решений порядок x1 и x2 не гарантируется.
 * @param a Коэффициент при квадратичном члене
 * @param b Коэффициент при линейном члене
 * @param c Свободный коэффициент
 * @param x1 Указатель на переменную для первого корня (при наличии)
 * @param x2 Указатель на переменную для второго корня (при наличии)
 * @return Количество найденных корней (0-2) или -1 если решений бесконечно много
 */
int solve_quad_eq(double a, double b, double c, double *x1, double *x2)
{
    // Уравнение является линейным
    if (fabs(a) < 2 * DBL_EPSILON)
    {
        return solve_lin_eq(b, c, x1);
    }
    else
    {
        double disc = pow(b, 2) - 4 * a * c;

        //Дискриминант равен нулю с учетом погрешности double арифметики (три операции при вычислении disc => 4 = 3+1)
        if (fabs(disc) < 4 * DBL_EPSILON)
        {
            *x1 = -b / (2 * a);
            return 1;
        }
        else if (disc < 0)
        {
            return 0;
        }
        else
        {
            *x1 = (-b + sqrt(disc)) / (2 * a);
            *x2 = (-b - sqrt(disc)) / (2 * a);
            return 2;
        }
    }
}

/**
 * Решает линейное уравнение вида kx + b = 0, записывает решение (при наличии) в переданную переменную (x)
 * и возвращает количество найденных решений или -1, если их бесконечно много.
 * @param k Коэффициент при линейном члене
 * @param b Свободный коэффициент
 * @param x Указатель на переменную для корня (при наличии)
 * @return Количество найденных корней или -1, если их бесконечно много
 */
int solve_lin_eq(double k, double b, double *x)
{
    // k = 0 и решений либо нет, либо бесконечно много (при 0=0)
    // 2 для большей уверенности
    if (fabs(k) < 2 * DBL_EPSILON)
    {
        if (fabs(b) < 2 * DBL_EPSILON)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        *x = -b / k;
        return 1;
    }
}