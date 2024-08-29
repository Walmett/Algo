#pragma once

#include <vector.h>
#include <sphere.h>
#include <intersection.h>
#include <triangle.h>
#include <ray.h>

#include <optional>

const double kEpsilon = 1e-7;

std::optional<Intersection> GetIntersection(const Ray& ray, const Sphere& sphere) {
    Vector center_vector = sphere.GetCenter() - ray.GetOrigin();
    double distance_projection = DotProduct(ray.GetDirection(), center_vector);
    if (distance_projection < 0 && Length(center_vector) > sphere.GetRadius()) {
        return std::nullopt;
    }
    Vector projection = ray.GetOrigin() + distance_projection * ray.GetDirection();
    double distance = Length(projection - sphere.GetCenter());
    if (distance > sphere.GetRadius()) {
        return std::nullopt;
    }
    double length_back = sqrt(sphere.GetRadius() * sphere.GetRadius() - distance * distance);
    if (Length(center_vector) > sphere.GetRadius()) {
        Vector position = projection - length_back * ray.GetDirection();
        Vector normal = position - sphere.GetCenter();
        distance = Length(position - ray.GetOrigin());
        return Intersection{position, normal, distance};
    } else {
        Vector position = projection + length_back * ray.GetDirection();
        Vector normal = sphere.GetCenter() - position;
        distance = Length(position - ray.GetOrigin());
        return Intersection{position, normal, distance};
    }
}

std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle) {
    Vector vertex0 = triangle[0];
    Vector vertex1 = triangle[1];
    Vector vertex2 = triangle[2];
    Vector edge1, edge2, h, s, q;
    double a, f, u, v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = CrossProduct(ray.GetDirection(), edge2);
    a = DotProduct(edge1, h);

    if (a > -kEpsilon && a < kEpsilon) {
        return std::nullopt;
    }

    f = 1.0 / a;
    s = ray.GetOrigin() - vertex0;
    u = f * DotProduct(s, h);

    if (u < 0.0 || u > 1.0) {
        return std::nullopt;
    }

    q = CrossProduct(s, edge1);
    v = f * DotProduct(ray.GetDirection(), q);

    if (v < 0.0 || u + v > 1.0) {
        return std::nullopt;
    }

    double t = f * DotProduct(edge2, q);

    if (t > kEpsilon) {
        Vector position = ray.GetOrigin() + ray.GetDirection() * t;
        Vector normal = CrossProduct(edge1, edge2);
        double distance = Length(position - ray.GetOrigin());
        if (DotProduct(ray.GetDirection(), normal) > 0) {
            normal = -1 * normal;
        }
        return Intersection{position, normal, distance};
    } else {
        return std::nullopt;
    }
}

Vector Reflect(const Vector& ray, const Vector& normal) {
    double cos = -DotProduct(ray, normal);
    Vector reflect = ray + 2 * cos * normal;
    return reflect;
}

std::optional<Vector> Refract(const Vector& ray, const Vector& normal, double eta) {
    double cos1 = -DotProduct(ray, normal);
    double sin1_sqr = 1 - cos1 * cos1;
    double sin2 = eta * sqrt(sin1_sqr);
    double cos2 = sqrt(1 - sin2 * sin2);
    if (sin2 > 1) {
        return std::nullopt;
    }
    Vector refract = eta * ray + (eta * cos1 - cos2) * normal;
    return refract;
}

Vector GetBarycentricCoords(const Triangle& triangle, const Vector& point) {
    double area = Length(CrossProduct(triangle[1] - triangle[0], triangle[2] - triangle[0]));
    double w1 = Length(CrossProduct(point - triangle[2], point - triangle[1])) / area;
    double w2 = Length(CrossProduct(point - triangle[0], point - triangle[2])) / area;
    double w3 = Length(CrossProduct(point - triangle[0], point - triangle[1])) / area;
    return {w1, w2, w3};
}
