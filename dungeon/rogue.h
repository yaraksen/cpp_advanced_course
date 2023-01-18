#pragma once

#include "dungeon.h"

#include <stdexcept>
#include <unordered_set>
#include <unordered_map>

void Dfs(Room* now, bool& exit_found, Room*& exit, std::unordered_set<std::string>& keys,
         std::unordered_map<Room*, bool> visited) {
    if (now == nullptr || exit_found) {
        return;
    }

    if (now->IsFinal()) {
        exit_found = true;
        exit = now;
        return;
    }

    visited[now] = true;

    for (size_t i = 0; i < now->NumKeys(); ++i) {
        keys.insert(now->GetKey(i));
    }
    for (size_t i = 0; i < now->NumDoors(); ++i) {
        if (now->GetDoor(i) && now->GetDoor(i)->IsOpen() && now->GetDoor(i)->GoThrough() &&
            !visited[reinterpret_cast<Room* const>(now->GetDoor(i)->GoThrough())]) {
            Dfs(now->GetDoor(i)->GoThrough(), exit_found, exit, keys, visited);
        } else if (now->GetDoor(i) && !now->GetDoor(i)->IsOpen()) {
            for (const std::string& key : keys) {
                if (now->GetDoor(i)->TryOpen(key)) {
                    break;
                }
            }
            if (now->GetDoor(i)->IsOpen() && now->GetDoor(i)->GoThrough() &&
                !visited[reinterpret_cast<Room* const>(now->GetDoor(i)->GoThrough())]) {
                Dfs(now->GetDoor(i)->GoThrough(), exit_found, exit, keys, visited);
            }
        }
    }
}

Room* FindFinalRoom(Room* starting_room) {
    std::unordered_set<std::string> keys;
    bool exit_found = false;
    Room* exit = nullptr;

    Dfs(starting_room, exit_found, exit, keys, std::unordered_map<Room*, bool>());

    return exit;
}
