// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2007 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fxbarcode/common/reedsolomon/BC_ReedSolomonGF256Poly.h"

#include <memory>
#include <utility>

#include "core/fxcrt/fx_system.h"
#include "fxbarcode/common/reedsolomon/BC_ReedSolomonGF256.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

CBC_ReedSolomonGF256Poly::CBC_ReedSolomonGF256Poly(
    CBC_ReedSolomonGF256* field,
    const std::vector<int32_t>& coefficients)
    : m_field(field) {
  ASSERT(m_field);
  ASSERT(!coefficients.empty());
  if (coefficients.size() == 1 || coefficients.front() != 0) {
    m_coefficients = coefficients;
    return;
  }

  size_t firstNonZero = 1;
  while (firstNonZero < coefficients.size() &&
         coefficients[firstNonZero] == 0) {
    firstNonZero++;
  }
  if (firstNonZero == coefficients.size()) {
    m_coefficients = m_field->GetZero()->GetCoefficients();
  } else {
    m_coefficients.resize(coefficients.size() - firstNonZero);
    for (size_t i = firstNonZero, j = 0; i < coefficients.size(); i++, j++)
      m_coefficients[j] = coefficients[i];
  }
}

CBC_ReedSolomonGF256Poly::~CBC_ReedSolomonGF256Poly() = default;

const std::vector<int32_t>& CBC_ReedSolomonGF256Poly::GetCoefficients() const {
  return m_coefficients;
}

int32_t CBC_ReedSolomonGF256Poly::GetDegree() const {
  return pdfium::CollectionSize<int32_t>(m_coefficients) - 1;
}

bool CBC_ReedSolomonGF256Poly::IsZero() const {
  return m_coefficients.front() == 0;
}

int32_t CBC_ReedSolomonGF256Poly::GetCoefficients(int32_t degree) const {
  return m_coefficients[m_coefficients.size() - 1 - degree];
}

std::unique_ptr<CBC_ReedSolomonGF256Poly> CBC_ReedSolomonGF256Poly::Clone()
    const {
  return pdfium::MakeUnique<CBC_ReedSolomonGF256Poly>(m_field.Get(),
                                                      m_coefficients);
}

std::unique_ptr<CBC_ReedSolomonGF256Poly>
CBC_ReedSolomonGF256Poly::AddOrSubtract(const CBC_ReedSolomonGF256Poly* other) {
  if (IsZero())
    return other->Clone();
  if (other->IsZero())
    return Clone();

  std::vector<int32_t> smallerCoefficients = m_coefficients;
  std::vector<int32_t> largerCoefficients = other->GetCoefficients();
  if (smallerCoefficients.size() > largerCoefficients.size())
    std::swap(smallerCoefficients, largerCoefficients);

  std::vector<int32_t> sumDiff(largerCoefficients.size());
  size_t lengthDiff = largerCoefficients.size() - smallerCoefficients.size();
  for (size_t i = 0; i < lengthDiff; ++i)
    sumDiff[i] = largerCoefficients[i];

  for (size_t i = lengthDiff; i < largerCoefficients.size(); ++i) {
    sumDiff[i] = CBC_ReedSolomonGF256::AddOrSubtract(
        smallerCoefficients[i - lengthDiff], largerCoefficients[i]);
  }
  return pdfium::MakeUnique<CBC_ReedSolomonGF256Poly>(m_field.Get(), sumDiff);
}

std::unique_ptr<CBC_ReedSolomonGF256Poly> CBC_ReedSolomonGF256Poly::Multiply(
    const CBC_ReedSolomonGF256Poly* other) {
  if (IsZero() || other->IsZero())
    return m_field->GetZero()->Clone();

  const std::vector<int32_t>& aCoefficients = m_coefficients;
  const std::vector<int32_t>& bCoefficients = other->GetCoefficients();
  size_t aLength = aCoefficients.size();
  size_t bLength = bCoefficients.size();
  std::vector<int32_t> product(aLength + bLength - 1);
  for (size_t i = 0; i < aLength; i++) {
    int32_t aCoeff = aCoefficients[i];
    for (size_t j = 0; j < bLength; j++) {
      product[i + j] = CBC_ReedSolomonGF256::AddOrSubtract(
          product[i + j], m_field->Multiply(aCoeff, bCoefficients[j]));
    }
  }
  return pdfium::MakeUnique<CBC_ReedSolomonGF256Poly>(m_field.Get(), product);
}

std::unique_ptr<CBC_ReedSolomonGF256Poly> CBC_ReedSolomonGF256Poly::Multiply(
    int32_t scalar) {
  if (scalar == 0)
    return m_field->GetZero()->Clone();
  if (scalar == 1)
    return Clone();

  size_t size = m_coefficients.size();
  std::vector<int32_t> product(size);
  for (size_t i = 0; i < size; i++)
    product[i] = m_field->Multiply(m_coefficients[i], scalar);

  return pdfium::MakeUnique<CBC_ReedSolomonGF256Poly>(m_field.Get(), product);
}

std::unique_ptr<CBC_ReedSolomonGF256Poly>
CBC_ReedSolomonGF256Poly::MultiplyByMonomial(int32_t degree,
                                             int32_t coefficient) const {
  if (degree < 0)
    return nullptr;
  if (coefficient == 0)
    return m_field->GetZero()->Clone();

  size_t size = m_coefficients.size();
  std::vector<int32_t> product(size + degree);
  for (size_t i = 0; i < size; i++)
    product[i] = m_field->Multiply(m_coefficients[i], coefficient);

  return pdfium::MakeUnique<CBC_ReedSolomonGF256Poly>(m_field.Get(), product);
}

std::unique_ptr<CBC_ReedSolomonGF256Poly> CBC_ReedSolomonGF256Poly::Divide(
    const CBC_ReedSolomonGF256Poly* other) {
  if (other->IsZero())
    return nullptr;

  auto quotient = m_field->GetZero()->Clone();
  if (!quotient)
    return nullptr;
  auto remainder = Clone();
  if (!remainder)
    return nullptr;

  int32_t denominatorLeadingTerm = other->GetCoefficients(other->GetDegree());
  Optional<int32_t> inverseDenominatorLeadingTeam =
      m_field->Inverse(denominatorLeadingTerm);
  if (!inverseDenominatorLeadingTeam.has_value())
    return nullptr;

  while (remainder->GetDegree() >= other->GetDegree() && !remainder->IsZero()) {
    int32_t degreeDifference = remainder->GetDegree() - other->GetDegree();
    int32_t scale =
        m_field->Multiply(remainder->GetCoefficients((remainder->GetDegree())),
                          inverseDenominatorLeadingTeam.value());
    auto term = other->MultiplyByMonomial(degreeDifference, scale);
    if (!term)
      return nullptr;
    auto iteratorQuotient = m_field->BuildMonomial(degreeDifference, scale);
    if (!iteratorQuotient)
      return nullptr;
    quotient = quotient->AddOrSubtract(iteratorQuotient.get());
    if (!quotient)
      return nullptr;
    remainder = remainder->AddOrSubtract(term.get());
    if (!remainder)
      return nullptr;
  }
  return remainder;
}
