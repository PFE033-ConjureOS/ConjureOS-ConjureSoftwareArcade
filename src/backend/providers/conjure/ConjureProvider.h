//
// Created by jonny on 2023-10-17.
//


#pragma once

#include "providers/Provider.h"


namespace providers {
    namespace conjure {
        class ConjureProvider : public Provider {
        Q_OBJECT

        public:
            explicit ConjureProvider(QObject *parent = nullptr);

            Provider &run(SearchContext &) final;
        };
    } // namespace conjure
} // namespace providers