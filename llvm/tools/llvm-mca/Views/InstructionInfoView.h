//===--------------------- InstructionInfoView.h ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
///
/// This file implements the instruction info view.
///
/// The goal fo the instruction info view is to print the latency and reciprocal
/// throughput information for every instruction in the input sequence.
/// This section also reports extra information related to the number of micro
/// opcodes, and opcode properties (i.e. 'MayLoad', 'MayStore', 'HasSideEffects)
///
/// Example:
///
/// Instruction Info:
/// [1]: #uOps
/// [2]: Latency
/// [3]: RThroughput
/// [4]: MayLoad
/// [5]: MayStore
/// [6]: HasSideEffects
///
/// [1]    [2]    [3]    [4]    [5]    [6]	Instructions:
///  1      2     1.00                    	vmulps	%xmm0, %xmm1, %xmm2
///  1      3     1.00                    	vhaddps	%xmm2, %xmm2, %xmm3
///  1      3     1.00                    	vhaddps	%xmm3, %xmm3, %xmm4
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TOOLS_LLVM_MCA_INSTRUCTIONINFOVIEW_H
#define LLVM_TOOLS_LLVM_MCA_INSTRUCTIONINFOVIEW_H

#include "Views/View.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MCA/CodeEmitter.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "llvm-mca"

namespace llvm {
namespace mca {

/// A view that prints out generic instruction information.
class InstructionInfoView : public InstructionView {
  const llvm::MCInstrInfo &MCII;
  CodeEmitter &CE;
  bool PrintEncodings;

  struct InstructionInfoViewData {
    unsigned NumMicroOpcodes = 0;
    unsigned Latency = 0;
    Optional<double> RThroughput = 0.0;
    bool mayLoad = false;
    bool mayStore = false;
    bool hasUnmodeledSideEffects = false;
  };
  using IIVDVec = SmallVector<InstructionInfoViewData, 16>;

  /// Place the data into the array of InstructionInfoViewData IIVD.
  void collectData(MutableArrayRef<InstructionInfoViewData> IIVD) const;

public:
  InstructionInfoView(const llvm::MCSubtargetInfo &ST,
                      const llvm::MCInstrInfo &II, CodeEmitter &C,
                      bool ShouldPrintEncodings, llvm::ArrayRef<llvm::MCInst> S,
                      llvm::MCInstPrinter &IP)
      : InstructionView(ST, IP, S), MCII(II), CE(C),
        PrintEncodings(ShouldPrintEncodings) {}

  void printView(llvm::raw_ostream &OS) const override;
};
} // namespace mca
} // namespace llvm

#endif
