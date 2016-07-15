/*
 * File:   plugboard.h
 * Author: dthiessen
 *
 * Created on June 30, 2016, 7:46 AM
 */

#ifndef PLUGBOARD_H
#define PLUGBOARD_H

struct plugboard_t {

    explicit plugboard_t(const char*p) : pairs(p) {
        for (int i = 0; i < 26; i++) {
            map[i] = i;
        }
        while (*p) {
            int a = *p++ -'A';
            int b = *p++ -'A';
            map[a] = b;
            map[b] = a;
            if (!*p++) break;
        }
    };
    const std::string pairs;
    char map[26];
};

#endif /* PLUGBOARD_H */

