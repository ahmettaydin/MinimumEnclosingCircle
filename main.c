#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

struct Point
{
    double x, y;
};

void createCircle(struct Point*, struct Point, struct Point, int, double[]);
int isValidCircle(struct Point*, struct Point, double, int);
void repositionPoints(struct Point*, int, double, int);
void drawCoordinatePlane(double, int, int, ALLEGRO_FONT*);
void drawCircle(struct Point, struct Point, double, int, int, double, ALLEGRO_FONT*);
void drawPoints(struct Point*, int, double);
void createSpline(struct Point*, int);

int main()
{
    FILE* file;

    if ((file = fopen("input.txt", "r")) == NULL)
    {
        printf("Dosya acilamadi");
        exit(1);
    }

    struct Point* points;
    struct Point center;
    struct Point furthestPoint;

    center.x = 0;
    center.y = 0;
    furthestPoint.x = 0;
    furthestPoint.y = 0;

    int n = 0;
    char ch;
    while (!feof(file))
    {
        ch = getc(file);
        if (ch == '\n')
            n++;
    }
    n++;

    rewind(file);

    points = (struct Point*)malloc(sizeof(struct Point) * n);

    for (int i = 0; i < n; i++)
    {
        fscanf(file, "%lf", &points[i].x);
        fscanf(file, "%lf", &points[i].y);
    }

    double values[] = { 0,0,0,0,0 };
    createCircle(points, center, furthestPoint, n, values);

    center.x = values[0];
    center.y = values[1];
    furthestPoint.x = values[2];
    furthestPoint.y = values[3];
    double radius = values[4];

    printf("r = %.4f\n", radius);

    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_DISPLAY* display = NULL;

    al_init();

    ALLEGRO_FONT* font15 = al_load_font("arial.ttf", 15, 0);
    ALLEGRO_FONT* font20 = al_load_font("arial.ttf", 20, 0);

    display = al_create_display(600, 600);
    al_clear_to_color(al_map_rgb(255, 255, 255));

    int gridSize = 30;
    int tileCount = 20;
    double halfWidth = tileCount * gridSize / 2;

    printf("M = {%.2f, %.2f}\n", center.x, center.y);
    al_draw_textf(font15, al_map_rgb(200, 0, 200), halfWidth + center.x * gridSize, halfWidth - center.y * gridSize,
        ALLEGRO_ALIGN_CENTER, "{%.2f, %.2f}", center.x, center.y);

    center.x = halfWidth + center.x * gridSize;
    center.y = halfWidth - center.y * gridSize;

    repositionPoints(points, n, halfWidth, gridSize);
    drawCoordinatePlane(halfWidth, gridSize, tileCount, font15);
    drawCircle(center, furthestPoint, radius, gridSize, tileCount, halfWidth, font20);

    createSpline(points, n);

    drawPoints(points, n, halfWidth);

    al_flip_display();

    system("pause");

    return 0;
}

void createCircle(struct Point* points, struct Point center, struct Point furthestPoint, int n, double values[])
{
    double radius = 100;

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            center.x = (points[i].x + points[j].x) / 2;
            center.y = (points[i].y + points[j].y) / 2;
            double tmp = sqrt(pow(center.x - points[i].x, 2) + pow(center.y - points[i].y, 2));

            if (tmp < radius && isValidCircle(points, center, tmp, n)) {
                furthestPoint = points[i];
                radius = tmp;
                values[0] = center.x;
                values[1] = center.y;
                values[2] = furthestPoint.x;
                values[3] = furthestPoint.y;
                values[4] = radius;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            for (int k = j + 1; k < n; k++) {
                double ax = points[j].x - points[i].x;
                double ay = points[j].y - points[i].y;
                double bx = points[k].x - points[i].x;
                double by = points[k].y - points[i].y;

                double A = pow(ax, 2) + pow(ay, 2);
                double B = pow(bx, 2) + pow(by, 2);
                double C = ax * by - ay * bx;

                center.x = (A * by - B * ay) / (C * 2) + points[i].x;
                center.y = (B * ax - A * bx) / (C * 2) + points[i].y;

                double tmp = sqrt(pow(center.x - points[i].x, 2) + pow(center.y - points[i].y, 2));

                if (tmp < radius && isValidCircle(points, center, tmp, n)) {
                    furthestPoint = points[i];
                    radius = tmp;
                    values[0] = center.x;
                    values[1] = center.y;
                    values[2] = furthestPoint.x;
                    values[3] = furthestPoint.y;
                    values[4] = radius;
                }
            }
        }
    }
}

int isValidCircle(struct Point* points, struct Point center, double radius, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (radius < sqrt(pow(center.x - points[i].x, 2) + pow(center.y - points[i].y, 2)))
        {
            return 0;
        }
    }
    return 1;
}

void repositionPoints(struct Point* points, int n, double screenWidth, int gridSize)
{
    for (int i = 0; i < n; i++)
    {
        points[i].x = screenWidth + points[i].x * gridSize;
        points[i].y = screenWidth - points[i].y * gridSize;
    }
}

void drawCoordinatePlane(double halfWidth, int gridSize, int tileCount, ALLEGRO_FONT* font15)
{
    al_draw_line(halfWidth, 0, halfWidth, gridSize * tileCount, al_map_rgb(0, 0, 0), 1);
    al_draw_line(0, halfWidth, gridSize * tileCount, halfWidth, al_map_rgb(0, 0, 0), 1);

    for (int i = 0; i <= tileCount; i++)
    {
        al_draw_filled_circle(halfWidth, i * gridSize, 2, al_map_rgb(0, 0, 0));
        al_draw_textf(font15, al_map_rgb(150, 150, 0), halfWidth - 5, i * gridSize - 9, ALLEGRO_ALIGN_RIGHT, "%i", tileCount / 2 - i);
        al_draw_filled_circle(i * gridSize, halfWidth, 2, al_map_rgb(0, 0, 0));
        al_draw_textf(font15, al_map_rgb(150, 150, 0), i * gridSize, halfWidth, ALLEGRO_ALIGN_CENTER, "%i", i - tileCount / 2);
    }
}

void drawCircle(struct Point center, struct Point furthestPoint, double radius, int gridSize, int tileCount, double halfWidth, ALLEGRO_FONT* font20)
{
    al_draw_circle(center.x, center.y, radius * gridSize, al_map_rgb(0, 180, 255), 2);
    al_draw_line((tileCount / 2 + furthestPoint.x) * gridSize, (tileCount / 2 - furthestPoint.y) * gridSize,
        center.x, center.y, al_map_rgb(0, 200, 200), 2);
    al_draw_filled_circle(center.x, center.y, 5, al_map_rgb(200, 0, 200));
    al_draw_textf(font20, al_map_rgb(0, 200, 200), 10, 0, 0, "r = %f", radius);
}

void createSpline(struct Point* points, int n)
{
    double xt = 0;
    double yt = 0;
    double t = 0;

    if (n < 2)
    {
    }
    else if (n == 2)
    {
        al_draw_line(points[0].x, points[0].y, points[1].x, points[1].y, al_map_rgb(200, 100, 0), 2);
    }
    else if (n == 3)
    {
        double x[3];
        double y[3];

        for (int i = 0; i < 3; i++) {
            x[i] = points[i].x;
            y[i] = points[i].y;
        }

        for (t = 0; t <= 1; t += 0.0001)
        {
            xt = pow(1 - t, 2) * x[0] + 2 * t * (1 - t) * x[1] + t * t * x[2];
            yt = pow(1 - t, 2) * y[0] + 2 * t * (1 - t) * y[1] + t * t * y[2];
            al_draw_filled_circle((int)xt, (int)yt, 1, al_map_rgb(200, 100, 0));
        }
    }
    else if (n >= 4)
    {
        double x[4];
        double y[4];

        for (int i = 0; i < 4; i++) {
            x[i] = points[i].x;
            y[i] = points[i].y;
        }

        for (t = 0; t <= 1; t += 0.0001)
        {
            xt = pow(1 - t, 3) * x[0] + 3 * t * pow(1 - t, 2) * x[1] + 3 * pow(t, 2) * (1 - t) * x[2]
                + pow(t, 3) * x[3];
            yt = pow(1 - t, 3) * y[0] + 3 * t * pow(1 - t, 2) * y[1] + 3 * pow(t, 2) * (1 - t) * y[2]
                + pow(t, 3) * y[3];
            al_draw_filled_circle((int)xt, (int)yt, 1, al_map_rgb(200, 100, 0));
        }
    }

}

void drawPoints(struct Point* points, int n, double halfWidth)
{
    for (int i = 0; i < n; i++)
    {
        al_draw_line(points[i].x, points[i].y, points[i].x, halfWidth, al_map_rgb(0, 0, 0), 1);
        al_draw_line(points[i].x, points[i].y, halfWidth, points[i].y, al_map_rgb(0, 0, 0), 1);
        al_draw_filled_circle(points[i].x, points[i].y, 5, al_map_rgb(255, 0, 0));
    }
}
