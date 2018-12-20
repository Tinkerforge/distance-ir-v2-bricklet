#!/usr/bin/env python

# Note: This code is copied from Distance IR Bricklet (1.0) Brick Viewer plugin.
#       The new Bricklet has enough flash to save all of the calibration data,
#       so we we can generate a lookup-table here and save the data in the Bricklet.

from __future__ import print_function

# Filename
filename = '2Y0A02.txt' # '2Y0A41.txt' / '2Y0A21.txt' / '2Y0A02.txt'

# Number of bits of input values
ANALOG_BITS = 12



NUM_VALUES = 512
DIVIDER = (2**ANALOG_BITS)//NUM_VALUES

class NaturalSpline(object):
    def __init__(self):
        self.points = []
        self.a = []
        self.b = []
        self.c = []

    def set_points(self, points):
        length = len(points)

        if length < 3:
            return False

        a = [0.0] * (length - 1)
        b = [0.0] * (length - 1)
        c = [0.0] * (length - 1)
        h = [0.0] * (length - 1)

        for i in range(length - 1):
            h[i] = points[i + 1][0] - points[i][0]

            if h[i] <= 0:
                return False

        d = [0.0] * (length - 1)
        dy1 = (points[1][1] - points[0][1]) / h[0]

        for i in range(1, length - 1):
            c[i] = h[i]
            b[i] = h[i]
            a[i] = 2.0 * (h[i - 1] + h[i])
            dy2 = (points[i + 1][1] - points[i][1]) / h[i]
            d[i] = 6.0 * (dy1 - dy2)
            dy1 = dy2

        for i in range(1, length - 2):
            c[i] /= a[i]
            a[i + 1] -= b[i] * c[i]

        s = [0.0] * length
        s[1] = d[1]

        for i in range(2, length - 1):
            s[i] = d[i] - c[i - 1] * s[i - 1]

        s[length - 2] = -s[length - 2] / a[length - 2]

        for i in reversed(range(1, length - 2)):
            s[i] = - ( s[i] + b[i] * s[i+1] ) / a[i]

        s[length - 1] = s[0] = 0.0

        for i in range(length - 1):
            a[i] = (s[i+1] - s[i]) / (6.0 * h[i])
            b[i] = 0.5 * s[i]
            c[i] = (points[i+1][1] - points[i][1]) / h[i] - (s[i + 1] + 2.0 * s[i]) * h[i] / 6.0

        self.points = points
        self.a = a
        self.b = b
        self.c = c

        return True

    def get_index(self, x):
        points = self.points
        length = len(points)

        if x <= points[0][0]:
            i1 = 0
        elif x >= points[length - 2][0]:
            i1 = length - 2
        else:
            i1 = 0
            i2 = length - 2
            i3 = 0

            while i2 - i1 > 1:
                i3 = i1 + ((i2 - i1) >> 1)

                if points[i3][0] > x:
                    i2 = i3
                else:
                    i1 = i3

        return i1

    def get_value(self, x):
        if len(self.a) == 0:
            return 0.0

        i = self.get_index(x)
        delta = x - self.points[i][0]

        return (((self.a[i] * delta) + self.b[i]) * delta + self.c[i]) * delta + self.points[i][1]


def sample_interpolate(x, y):
    spline = NaturalSpline()
    points = []

    for point in zip(x, y):
        points.append((float(point[0]), float(point[1])))

    spline.set_points(points)

    px = range(0, 2**ANALOG_BITS, DIVIDER)
    py = []

    for X in px:
        py.append(spline.get_value(X))

    for i in range(x[0]//DIVIDER):
        py[i] = y[0]

    for i in range(x[-1]//DIVIDER, 2**ANALOG_BITS//DIVIDER):
        py[i] = y[-1]

    for i in range(len(py)):
        if py[i] > y[0]:
            py[i] = y[0]
        if py[i] < y[-1]:
            py[i] = y[-1]

    print()
    for i in range(NUM_VALUES):
        value = int(round(py[i]*100))
        print('{0}, '.format(value), end='')
    print()

x = []
y = []

with open(filename, 'r') as f:
    for line in f:
        c = line.find('#')
        if c != -1:
            line = line[:c]

        line = line.strip()

        if line.startswith('\xEF\xBB\xBF'): # strip UTF-8 BOM, Internet Explorer adds it to text files
            line = line[3:]

        if len(line) == 0:
            continue

        if ':' not in line:
            print('Sample points file is malformed (error code 1)')
            break

        s = line.split(':')

        if len(s) != 2:
            print('Sample points file is malformed (error code 2)')
            break

        try:
            x.append(int(s[1]))
            y.append(int(s[0]))
        except:
            print('Sample points file is malformed (error code 3)')
            break

    sample_interpolate(x, y)
