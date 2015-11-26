/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 Dmitry "Dima" Korolev <dmitry.korolev@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#ifndef SCHEMA_H
#define SCHEMA_H

#include "../../TypeSystem/Reflection/schema.h"
#include "../../Bricks/time/chrono.h"

using namespace current;

// Storage schema, for persistence, publish, and subscribe.

CURRENT_STRUCT(UserAdded) {
  CURRENT_FIELD(user_id, std::string);
  CURRENT_FIELD(nickname, std::string);
};

CURRENT_STRUCT(PostAdded) {
  CURRENT_FIELD(post_id, std::string);
  CURRENT_FIELD(content, std::string);
  CURRENT_FIELD(author_user_id, std::string);
};

CURRENT_STRUCT(UserLike) {
  CURRENT_FIELD(user_id, std::string);
  CURRENT_FIELD(post_id, std::string);
};

// `Event` is the top-level message to persist.
CURRENT_STRUCT(Nop){};  // TODO(dkorolev): Will go away, see below.
CURRENT_STRUCT(Event) {
  CURRENT_FIELD(timestamp, uint64_t);
  CURRENT_FIELD(event, (Polymorphic<Nop, UserAdded, PostAdded, UserLike>));

  // TODO(dkorolev): This will go away.
  // Default construction is required for incoming JSON serialization as of now,
  // and `Polymorphic` is strict to not be left uninitialized.
  CURRENT_DEFAULT_CONSTRUCTOR(Event) : event(Nop()) {}

  // TODO(dkorolev): This will go away.
  // Moving forward, `EPOCH_MICROSECONDS` is the [unique] key,
  // and just keeping a `CURRENT_FIELD(timestamp, EPOCH_MICROSECONDS)` top-level field should be sufficient.
  EPOCH_MILLISECONDS ExtractTimestamp() const { return static_cast<EPOCH_MILLISECONDS>(timestamp); }
};

// JSON responses schema.

CURRENT_STRUCT(UserNickname) {
  CURRENT_FIELD(user_id, std::string);
  CURRENT_FIELD(nickname, std::string);

  CURRENT_CONSTRUCTOR(UserNickname)(const std::string& user_id, const std::string& nickname)
      : user_id(user_id), nickname(nickname) {}
};

CURRENT_STRUCT(UserNicknameNotFound) {
  CURRENT_FIELD(user_id, std::string);
  CURRENT_FIELD(error, std::string, "User not found.");

  CURRENT_CONSTRUCTOR(UserNicknameNotFound)(const std::string& user_id) : user_id(user_id) {}
};

CURRENT_STRUCT(Error) {
  CURRENT_FIELD(error, std::string);
  CURRENT_CONSTRUCTOR(Error)(const std::string& error = "Internal error.") : error(error) {}
};

#endif  // SCHEMA_H
