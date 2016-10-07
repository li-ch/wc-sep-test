#!/usr/local/bin/python

with open('../memac.trace', 'rb') as readfile:
    for l in readfile:
        ls = l.split(' ')
        if len(ls) > 3:
            print ls[3]

