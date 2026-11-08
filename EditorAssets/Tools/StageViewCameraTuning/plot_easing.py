#!/usr/bin/env python3
"""Sample stage camera easing curves (mirrors StageViewCameraEasing.h)."""

from __future__ import annotations

import math


def pull_back_reveal(t: float) -> float:
    t = max(0.0, min(1.0, t))
    if t >= 1.0:
        return 1.0
    if t < 0.35:
        u = t / 0.35
        return 0.08 * u * u
    u = (t - 0.35) / 0.65
    return 0.08 + 0.92 * (u * u * (3.0 - 2.0 * u))


def focus_push(t: float) -> float:
    k = 4.2
    t = max(0.0, min(1.0, t))
    if t <= 0.0:
        return 0.0
    if t >= 1.0:
        return 1.0
    denom = 1.0 - math.exp(-k)
    return (1.0 - math.exp(-k * t)) / denom


def in_out_quad(t: float) -> float:
    t = max(0.0, min(1.0, t))
    if t < 0.5:
        return 2.0 * t * t
    return 1.0 - (-2.0 * t + 2.0) * (-2.0 * t + 2.0) / 2.0


def main() -> None:
    steps = 20
    print("t\tpull_back\tfocus_push\tin_out_quad")
    for i in range(steps + 1):
        t = i / steps
        print(
            f"{t:.2f}\t{pull_back_reveal(t):.4f}\t"
            f"{focus_push(t):.4f}\t{in_out_quad(t):.4f}"
        )


if __name__ == "__main__":
    main()
