#pragma once

#include "Umbra/types.hpp"

namespace umbra {

  struct UMBRA_API SinglyLinkedList final : IType {
  private:

    struct SinglyLinkedListNode final {
      sol::object data;
      std::unique_ptr<SinglyLinkedListNode> next;

      explicit SinglyLinkedListNode(sol::object data) : data(std::move(data)), next(nullptr) {}
    };

    std::unique_ptr<SinglyLinkedListNode> head_;
    SinglyLinkedListNode* tail_;
    size_t size_;

    SinglyLinkedListNode* nth(size_t index) const noexcept {
      SinglyLinkedListNode* current = head_.get();
      while (current && index--) {
        current = current->next.get();
      }

      return current;
    }

  public:

    const char* name() override { return "SinglyLinkedList"; }

    SinglyLinkedList() noexcept : head_(nullptr), tail_(nullptr), size_(0) {}

    int size() const noexcept { return static_cast<int>(size_); }
    bool empty() const noexcept { return size_ <= 0; }

    void clear() noexcept {
      head_.reset();
      tail_ = nullptr;
      size_ = 0;
    }

    void push_front(sol::object value) {
      auto node = std::make_unique<SinglyLinkedListNode>(std::move(value));
      if (!head_) {
        // ReSharper disable once CppDFALocalValueEscapesFunction
        tail_ = node.get();
        head_ = std::move(node);
      } else {
        node->next = std::move(head_);
        head_ = std::move(node);
      }

      size_++;
    }

    void push_back(sol::object value) {
      auto node = std::make_unique<SinglyLinkedListNode>(std::move(value));
      SinglyLinkedListNode* raw = node.get();
      if (!tail_) {
        head_ = std::move(node);
        // ReSharper disable once CppDFALocalValueEscapesFunction
        tail_ = raw;
      } else {
        tail_->next = std::move(node);
        // ReSharper disable once CppDFALocalValueEscapesFunction
        tail_ = raw;
      }

      size_++;
    }

    sol::object pop_front(const sol::this_state this_state) noexcept {
      if (!head_) {
        return make_object(this_state, sol::lua_nil);
      }

      sol::object out = std::move(head_->data);
      head_ = std::move(head_->next);

      if (!head_) {
        tail_ = nullptr;
      }

      size_--;
      return out;
    }

    sol::object pop_back(const sol::this_state this_state) noexcept {
      if (!head_) {
        return make_object(this_state, sol::lua_nil);
      }

      if (head_.get() == tail_) {
        sol::object out = std::move(head_->data);
        clear();
        return out;
      }

      SinglyLinkedListNode* previous = head_.get();
      while (previous->next.get() != tail_) {
        previous = previous->next.get();
      }

      sol::object out = std::move(tail_->data);
      previous->next.reset();
      tail_ = previous;

      size_--;
      return out;
    }

    sol::object get(const int index, const sol::this_state this_state) const noexcept {
      if (index < 1 || static_cast<size_t>(index) > size_) {
        return make_object(this_state, sol::lua_nil);
      }

      SinglyLinkedListNode* n = nth(index - 1);
      return n ? n->data : make_object(this_state, sol::lua_nil);
    }

    void set(const int index, sol::object value) const noexcept {
      if (index < 1 || index > size_) {
        return;
      }

      if (SinglyLinkedListNode* n = nth(index - 1)) {
        n->data = std::move(value);
      }
    }

    void insert(const int index, sol::object value) {
      const size_t n = size_;
      if (index <= 1) {
        push_front(std::move(value));
        return;
      }

      if (index > n + 1) {
        push_back(std::move(value));
        return;
      }

      SinglyLinkedListNode* previous = nth(index - 2);

      auto node = std::make_unique<SinglyLinkedListNode>(std::move(value));
      SinglyLinkedListNode* raw = node.get();
      node->next = std::move(previous->next);
      previous->next = std::move(node);
      if (!raw->next) {
        // ReSharper disable once CppDFALocalValueEscapesFunction
        tail_ = raw;
      }

      size_++;
    }

    void erase(const int index) noexcept {
      if (index < 1 || index > size_) {
        return;
      }

      if (index == 1) {
        if (head_) {
          head_ = std::move(head_->next);
          if (!head_) {
            tail_ = nullptr;
          }
          size_--;
        }

        return;
      }

      SinglyLinkedListNode* previous = nth(index - 2);
      if (!previous || !previous->next) {
        return;
      }

      if (previous->next.get() == tail_) {
        tail_ = previous;
      }

      previous->next = std::move(previous->next->next);
      size_--;
    }

    SinglyLinkedList concat(const SinglyLinkedList& other) const {
      SinglyLinkedList out;

      for (const SinglyLinkedListNode* current = head_.get(); current; current = current->next.get()) {
        out.push_back(current->data);
      }

      for (const SinglyLinkedListNode* current = other.head_.get(); current; current = current->next.get()) {
        out.push_back(current->data);
      }

      return out;
    }

    static SinglyLinkedList from_table(const sol::table& table) {
      SinglyLinkedList out;

      const size_t length = table.size();
      for (size_t i = 1; i <= length; ++i) {
        out.push_back(table.get<sol::object>(i));
      }

      return out;
    }

    sol::as_table_t<std::vector<sol::object>> to_table() const {
      std::vector<sol::object> vector;
      vector.reserve(size_);

      for (SinglyLinkedListNode* current = head_.get(); current; current = current->next.get()) {
        vector.emplace_back(current->data);
      }

      return as_table(std::move(vector));
    }

    bool equals(const SinglyLinkedList& other, const sol::this_state this_state) const {
      if (size_ != other.size_) {
        return false;
      }

      sol::state_view state_view(this_state);
      const sol::function raw_equal = state_view["rawequal"];

      SinglyLinkedListNode* a = head_.get();
      SinglyLinkedListNode* b = other.head_.get();

      while (a && b) {
        if (raw_equal.valid()) {
          if (!raw_equal(a->data, b->data)) {
            return false;
          }
        } else {
          if (a->data != b->data) {
            return false;
          }
        }

        a = a->next.get();
        b = b->next.get();
      }

      return true;
    }

    std::string to_string(const sol::this_state this_state) const {
      sol::state_view state_view(this_state);
      const sol::function to_string = state_view["tostring"];
      std::ostringstream string_stream;

      string_stream << "SinglyLinkedList(";
      size_t i = 0;
      for (SinglyLinkedListNode* current = head_.get(); current; current = current->next.get(), ++i) {
        if (i) {
          string_stream << ", ";
        }

        if (current->data.is<std::string>()) {
          string_stream << '\'' << current->data.as<std::string>() << '\'';
          continue;
        }

        if (to_string.valid()) {
          sol::object value = to_string(current->data);
          if (value.is<std::string>()) {
            string_stream << value.as<std::string>();
          } else {
            string_stream << "<obj>";
          }
        } else {
          string_stream << "<obj>";
        }
      }
      string_stream << ")";

      return string_stream.str();
    }

    void bind(sol::state& lua_state) {
      sol::usertype<SinglyLinkedList> user_type = lua_state.new_usertype<SinglyLinkedList>(name(),
        sol::constructors<SinglyLinkedList()>(),
        "size", &SinglyLinkedList::size,
        "empty", &SinglyLinkedList::empty,
        "clear", &SinglyLinkedList::clear,
        "push_front", &SinglyLinkedList::push_front,
        "push_back", &SinglyLinkedList::push_back,
        "pop_front", &SinglyLinkedList::pop_front,
        "pop_back", &SinglyLinkedList::pop_back,
        "get", &SinglyLinkedList::get,
        "set", &SinglyLinkedList::set,
        "insert", &SinglyLinkedList::insert,
        "erase", &SinglyLinkedList::erase,
        "from_table", &SinglyLinkedList::from_table,
        "to_table", &SinglyLinkedList::to_table
      );

      user_type[sol::meta_function::length] = [](const SinglyLinkedList& singly_linked_list) {
        return singly_linked_list.size();
      };

      user_type[sol::meta_function::addition] = [](const SinglyLinkedList& first, const SinglyLinkedList& second) {
        return first.concat(second);
      };

      user_type[sol::meta_function::equal_to] = [](const SinglyLinkedList& first, const SinglyLinkedList& second, const sol::this_state this_state) {
        return first.equals(second, this_state);
      };

      user_type[sol::meta_function::to_string] = [](const SinglyLinkedList& singly_linked_list, const sol::this_state this_state) {
        return singly_linked_list.to_string(this_state);
      };

      user_type[sol::meta_function::index] = [](const SinglyLinkedList& singly_linked_list, const sol::stack_object key, const sol::this_state this_state) {
        if (key.is<int>()) {
          return singly_linked_list.get(key.as<int>(), this_state);
        }

        if (key.is<double>()) {
          return singly_linked_list.get(static_cast<int>(key.as<double>()), this_state);
        }

        return make_object(this_state, sol::lua_nil);
      };

      user_type[sol::meta_function::ipairs] = [](const SinglyLinkedList& singly_linked_list, const sol::this_state this_state) {
        struct State { SinglyLinkedListNode* current; int i; };
        auto state = std::make_shared<State>(State{ singly_linked_list.head_.get(), 0 });

        auto iter = [state, this_state](sol::object, sol::object) -> std::tuple<sol::object, sol::object> {
          if (!state->current) {
            sol::object nil = make_object(this_state, sol::lua_nil);
            return { nil, nil };
          }

          state->i += 1;
          sol::object index = make_object(this_state, sol::lua_nil);
          sol::object value = state->current->data;

          state->current = state->current->next.get();

          if (value.is<sol::lua_nil_t>()) {
            sol::object nil = make_object(this_state, sol::lua_nil);
            return { nil, nil };
          }

          return { index, value };
        };

        return std::make_tuple(iter, make_object(this_state, sol::lua_nil), 0);
      };

      user_type[sol::meta_function::pairs] = [](const SinglyLinkedList& singly_linked_list, const sol::this_state this_state) {
        struct State { SinglyLinkedListNode* current; int i; };
        auto state = std::make_shared<State>(State{ singly_linked_list.head_.get(), 0 });

        auto iter = [state, this_state](sol::object, sol::object) -> std::tuple<sol::object, sol::object> {
          if (!state->current) {
            sol::object nil = make_object(this_state, sol::lua_nil);
            return { nil, nil };
          }

          state->i += 1;
          sol::object index = sol::make_object(this_state, state->i);
          sol::object value = state->current->data;
          state->current = state->current->next.get();

          return { index, value };
        };

        return std::make_tuple(iter, make_object(this_state, sol::lua_nil), 0);
      };

      user_type[sol::meta_function::concatenation] = [](const SinglyLinkedList& first, const SinglyLinkedList& second) {
        return first.concat(second);
      };
    }

  };

}