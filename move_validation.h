#ifndef MOVE_VALIDATION_H
#define MOVE_VALIDATION_H

bool is_valid_move_pawn_white(const Move *, const Move *);

bool is_valid_move_pawn_black(const Move *, const Move *);

bool is_valid_move_rook(const Move *, const Move *);

bool is_valid_move_knight(const Move *, const Move *);

bool is_valid_move_bishop(const Move *, const Move *);

bool is_valid_move_queen(const Move *, const Move *);

bool is_valid_move_king_white(const Move *, const Move *);

bool is_valid_move_king_black(const Move *, const Move *);

bool is_valid_move_empty(const Move *, const Move *);

#endif