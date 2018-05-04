#ifndef SCOPE_H
#define SCOPE_H

#include "Symbol.h"

namespace px {

    class Scope
    {
    public:
        Scope(Scope *parent = nullptr) : parent_{ parent }, childIndex_{ 0 }
        {
            bool nullParent = parent == nullptr;
            SymbolTable *parentTable = !nullParent ? parent->symbols_.get() : nullptr;
            symbols_.reset(new SymbolTable{ parentTable });
            if (nullParent)
                symbols_->addGlobals();
            else
                parent->children_.push_back(this);
        }

        ~Scope()
        {
            for (auto child : children_)
            {
                delete child;
            }
        }

        Scope * parent() const
        {
            return parent_;
        }

        SymbolTable *symbols() const
        {
            return symbols_.get();
        }

        Scope *enterScope()
        {
            return children_[childIndex_];
        }

        void leaveScope()
        {
            ++childIndex_;
        }

    private:
        std::unique_ptr<SymbolTable> symbols_;
        Scope * const parent_;
        std::vector<Scope*> children_;
        size_t childIndex_;
    };

    class ScopeTree
    {
    public:
        ScopeTree()
        {
            root_ = current_ = new Scope();
        }

        Scope *current() const
        {
            return current_;
        }

        Scope *enterScope()
        {
            auto child = current_->enterScope();
            current_ = child;
            return current_;
        }

        void endScope()
        {
            current_ = current_->parent();
            current_->leaveScope();
        }

    private:
        Scope * root_, *current_;
    };

}

#endif
