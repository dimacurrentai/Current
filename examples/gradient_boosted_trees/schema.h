/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2017 Dmitry "Dima" Korolev <dmitry.korolev@gmail.com>

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

#ifndef EXAMPLES_GRADIENT_BOOSTED_TREES_GBT_SCHEMA_H
#define EXAMPLES_GRADIENT_BOOSTED_TREES_GBT_SCHEMA_H

#include "../../current.h"

CURRENT_STRUCT(InputOfBinaryLabelsAndBinaryFeatures) {
  // Label of each example, 0/1.
  CURRENT_FIELD(labels, std::vector<uint8_t>);

  // Name of each indexed example, optional. If present, `point_names.size()` == `labels.size()`.
  CURRENT_FIELD(point_names, Optional<std::vector<std::string>>);

  // Name of each indexed feature.
  CURRENT_FIELD(features, std::vector<std::string>);

  // Sorted list of features present in example `i`, `matrix.size()` == `points.size()`.
  CURRENT_FIELD(matrix, std::vector<std::vector<uint32_t>>);
};

CURRENT_STRUCT(TreeNode) {
  CURRENT_FIELD(leaf, bool, true);

  // Used only when `leaf` is true: The value corresponding to this leaf.
  CURRENT_FIELD(value, double, 0.0);

  // Used only when `leaf` is false: The index of the (binary) feature to split by, and the indexes of the two subtrees.
  CURRENT_FIELD(feature, uint32_t, static_cast<uint32_t>(-1));
  CURRENT_FIELD(yes, uint32_t, static_cast<uint32_t>(-1));
  CURRENT_FIELD(no, uint32_t, static_cast<uint32_t>(-1));
};

// Class `TreeBuilder` implements an efficient way to build a tree.
CURRENT_ENUM(TreeIndex, size_t){Invalid = static_cast<size_t>(-1)};

CURRENT_STRUCT(TreeEnsemble) {
  CURRENT_FIELD(nodes, std::vector<TreeNode>);
  CURRENT_FIELD(trees, std::vector<TreeIndex>);  // Indexes of tree roots.
};

#endif  // EXAMPLES_GRADIENT_BOOSTED_TREES_GBT_SCHEMA_H
