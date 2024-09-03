#pragma once

#include <cmath>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "color.hpp"

/**
 * Static class for shapes rendering
 * Not meant for continuous rendering
 */
class Shape {
    static constexpr double SQRT2_2 = M_SQRT2 / 2;

public:
    static void circle(
        SDL_Renderer *renderer,
        const float x,
        const float y,
        const float radius,
        const Color::Name color
    ) {
        const auto [r, g, b, a] = Color::get(color).get_rgb();

        aaFilledEllipseRGBA(renderer, x + radius, y + radius, radius, radius, r, g, b, a);
    }

    static void circumference(
        SDL_Renderer *renderer,
        const float x,
        const float y,
        const float thickness,
        const float radius,
        const Color::Name color
    ) {
        const auto [r, g, b, a] = Color::get(color).get_rgb();
        const float real_radius = radius - thickness / 2;

        aaArcRGBA(renderer, x + radius, y + radius, real_radius, real_radius, 0, 360, thickness, r, g, b, a);
    }

    static void circle_sector(
        SDL_Renderer *renderer,
        const float x,
        const float y,
        const float radius,
        const float from,
        const float to,
        const Color::Name color
    ) {
        const auto [r, g, b, a] = Color::get(color).get_rgb();

        aaFilledPieRGBA(renderer, x + radius, y + radius, radius, radius, from, to, 0, r, g, b, a);
    }

    static void filled_rectangle(SDL_Renderer *renderer, const SDL_Rect &rectangle, const SDL_Color &color) {
        const auto [r, g, b, a] = color;

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderFillRect(renderer, &rectangle);
    }

    static void filled_rectangle(SDL_Renderer *renderer, const SDL_Rect &rectangle, const Color::Name color) {
        filled_rectangle(renderer, rectangle, Color::get(color).get_rgb());
    }

    static void filled_rounded_rectangle(
        SDL_Renderer *renderer,
        const SDL_Rect &rectangle,
        float border_thickness,
        const float radius,
        const Color::Name color,
        const Color::Name border_color
    ) {
        const auto [x, y, w, h] = rectangle;
        const auto [r, g, b, a] = Color::get(color).get_rgb();

        if (border_thickness > 0 && border_thickness < 1)
            border_thickness = 1;

        const int radius_int = radius;
        const int fill_padding = border_thickness / 2;
        const double fill_radius = radius - border_thickness / 2;

        const SDL_Rect rect_filling[3] = {
            {x + fill_padding, y + radius_int, w - fill_padding * 2, h - radius_int * 2}, // middle
            {x + radius_int, y + fill_padding, w - radius_int * 2, radius_int - fill_padding}, // top
            {
                x + radius_int,
                y + h - radius_int - fill_padding,
                w - radius_int * 2,
                radius_int - fill_padding
            }, // bottom
        };

        const SDL_FPoint corners_centers[4] = {
            {x + w - radius, y + h - radius}, // bottom right
            {x + radius, y + h - radius}, // bottom left
            {x + radius, y + radius}, // top left
            {x + w - radius, y + radius}, // top right
        };

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderFillRects(renderer, rect_filling, 4);

        for (int i = 0; i < 4; i++) {
            const auto &[x, y] = corners_centers[i];
            aaFilledPieRGBA(
                renderer,
                x,
                y,
                fill_radius,
                fill_radius,
                90 * i,
                90 * (i + 1),
                0,
                r,
                g,
                b,
                a
            );
        }

        rounded_rectangle(renderer, rectangle, border_thickness, radius, border_color);
    }

    static void rounded_rectangle(
        SDL_Renderer *renderer,
        const SDL_Rect &rectangle,
        float thickness,
        const float radius,
        const Color::Name color
    ) {
        const auto [x, y, w, h] = rectangle;
        const auto [r, g, b, a] = Color::get(color).get_rgb();

        if (thickness > 0 && thickness < 1)
            thickness = 1;

        const int radius_int = radius;
        const int thickness_int = thickness;
        const double arc_radius = radius - thickness / 2;

        const SDL_Rect borders[4] = {
            {x, y + radius_int, thickness_int, h - radius_int * 2}, // left
            {x + w - thickness_int, y + radius_int, thickness_int, h - radius_int * 2}, // right
            {x + radius_int, y, w - radius_int * 2, thickness_int}, // top
            {x + radius_int, y + h - thickness_int, w - radius_int * 2, thickness_int}, // bottom
        };

        const SDL_FPoint corners_centers[4] = {
            {x + w - radius, y + h - radius}, // bottom right
            {x + radius, y + h - radius}, // bottom left
            {x + radius, y + radius}, // top left
            {x + w - radius, y + radius}, // top right
        };

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderFillRects(renderer, borders, 4);

        for (int i = 0; i < 4; i++) {
            const auto &[x, y] = corners_centers[i];
            aaArcRGBA(
                renderer,
                x,
                y,
                arc_radius,
                arc_radius,
                90 * i,
                90 * (i + 1),
                thickness,
                r,
                g,
                b,
                a
            );
        }
    }

    static void rounded_line(
        SDL_Renderer *renderer,
        const float x1,
        const float y1,
        const float x2,
        const float y2,
        const float thickness,
        const Color::Name color
    ) {
        const auto [r, g, b, a] = Color::get(color).get_rgb();

        if (thickness <= 1) {
            aalineRGBA(renderer, x1, y1, x2, y2, r, g, b, a);
            return;
        }

        /*
         * steup:
         * A = (x1, y1)
         * B = (x2, y2)
         *
         *   -- D --------------- H --
         * /    |                 |    \
         * A -- C --------------- F -- B
         * \    |                 |    /
         *   -- E --------------- G --
         */

        const double t_halfs = thickness / 2;
        const double t_sqrt2_2 = SQRT2_2 * thickness;
        const double radius = t_halfs * 0.99;
        const double line_angle = atan2(x1 - x2, y1 - y2);
        const double left_angle = line_angle - M_PI_4;
        const double right_angle = line_angle + M_PI_4;

        const double t_halfs_sin_a1 = t_halfs * sin(line_angle);
        const double t_halfs_cos_a1 = t_halfs * cos(line_angle);
        const double t_sqrt2_2_sin_a2 = t_sqrt2_2 * sin(left_angle);
        const double t_sqrt2_2_cos_a2 = t_sqrt2_2 * cos(left_angle);
        const double t_sqrt2_2_sin_a3 = t_sqrt2_2 * sin(right_angle);
        const double t_sqrt2_2_cos_a3 = t_sqrt2_2 * cos(right_angle);

        // start points C D E
        const double cx = x1 + t_halfs_sin_a1;
        const double cy = y1 + t_halfs_cos_a1;
        const double dx = x1 + t_sqrt2_2_sin_a2;
        const double dy = y1 + t_sqrt2_2_cos_a2;
        const double ex = x1 + t_sqrt2_2_sin_a3;
        const double ey = y1 + t_sqrt2_2_cos_a3;

        // end points F G H
        const double fx = x2 - t_halfs_sin_a1;
        const double fy = y2 - t_halfs_cos_a1;
        const double gx = x2 - t_sqrt2_2_sin_a2;
        const double gy = y2 - t_sqrt2_2_cos_a2;
        const double hx = x2 - t_sqrt2_2_sin_a3;
        const double hy = y2 - t_sqrt2_2_cos_a3;

        // build straight part
        const double line_x_points[] = {dx, hx, gx, ex};
        const double line_y_points[] = {dy, hy, gy, ey};

        aaFilledPolygonRGBA(renderer, line_x_points, line_y_points, 4, r, g, b, a);
        aaFilledEllipseRGBA(renderer, cx, cy, radius, radius, r, g, b, a);
        aaFilledEllipseRGBA(renderer, fx, fy, radius, radius, r, g, b, a);
    }
};
