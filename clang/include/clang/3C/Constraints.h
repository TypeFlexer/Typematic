//=--Constraints.h------------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This implements a simple constraint solver for expressions of the form:
//  a >= b
//
// The 3C tool performs type inference to identify locations
// where a C type might be replaced with a Checked C type. This interface
// does the solving to figure out where those substitions might happen.
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_3C_CONSTRAINTS_H
#define LLVM_CLANG_3C_CONSTRAINTS_H

#include "clang/3C/Utils.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <set>

class Constraint;
class TaintedConstraint;
class ConstraintVariable;
class Constraints;
class PersistentSourceLoc;
class ConstraintsGraph;
class TaintedStructureAtom;

#define INTERNAL_USE_REASON "This reason should never be displayed"
#define DEFAULT_REASON "UNKNOWN_REASON"
#define POINTER_IS_ARRAY_REASON "Pointer is array but alltypes is disabled."
#define VOID_TYPE_REASON "Default void* type"
#define UNWRITABLE_REASON "Source code in non-writable file."
#define INNER_POINTER_REASON "Pointer is within an outer pointer"
#define ALLOCATOR_REASON "Return type from an allocator"
#define ARRAY_REASON "Lowerbounded to an array type"
#define NT_ARRAY_REASON "Lowerbounded to an nt_array type"
// SPECIAL_REASON("name_of_func")
#define SPECIAL_REASON(spec) (std::string("Special case for ") + (spec))
#define STRING_LITERAL_REASON "The type of a string literal"
#define MACRO_REASON "Pointer in Macro declaration."
#define UNION_FIELD_REASON "Union field encountered"

template <typename T> struct PComp {
  bool operator()(const T Lhs, const T Rhs) const { return *Lhs < *Rhs; }
};

class VarAtom;

typedef std::map<VarAtom*, double> InfluenceMap;
typedef std::map<VarAtom*, clang::DeclaratorDecl*> InfluenceRootMap;
// Represents atomic values that can occur at positions in constraints.
class Atom {
public:
  enum AtomKind { A_Var, A_Ptr, A_Arr, A_NTArr, A_TPtr, A_TArr, A_TNTArr, A_Wild, A_Const, A_struct, A_Tstruct, A_DTstruct };

private:
  AtomKind Kind;
  clang::DeclaratorDecl *D = nullptr;

public:
  Atom(AtomKind K) : Kind(K) {}
  virtual ~Atom() {}

  AtomKind getKind() const { return Kind; }

  void setKind(AtomKind aK) {
      Kind = aK;
  }

  void setRoot(clang::DeclaratorDecl* decl)
  {
      D = decl;
  }

  clang::DeclaratorDecl* getRoot()
  {
        return D;
  }

  void reflectToTaintedKind()
  {
        switch (Kind) {
            case A_Ptr:
                Kind = A_TPtr;
                break;
            case A_Arr:
                Kind = A_TArr;
                break;
            case A_NTArr:
                Kind = A_TNTArr;
                break;
            case A_struct:
                Kind = A_Tstruct;
                break;
            default:
                break;
        }
  }

  bool isTainted() const {
      return (Kind == A_TPtr || Kind == A_TArr || Kind == A_TNTArr
      || Kind == A_Tstruct || Kind == A_DTstruct);
  }

  bool isStructure() const{
      return (Kind == A_struct);
  }

  bool isTaintedStructure() const {
      return (Kind == A_Tstruct);
  }

  virtual void print(llvm::raw_ostream &) const = 0;
  virtual void dump(void) const = 0;
  virtual void dumpJson(llvm::raw_ostream &) const = 0;
  std::string getStr() {
    std::string Buf;
    llvm::raw_string_ostream TmpS(Buf);
    print(TmpS);
    return TmpS.str();
  }
  virtual bool operator==(const Atom &) const = 0;
  virtual bool operator!=(const Atom &) const = 0;
  virtual bool operator<(const Atom &) const = 0;
};

class ConstAtom : public Atom {
public:
  ConstAtom() : Atom(A_Const) {}
  ConstAtom(AtomKind K) : Atom(K) {}

  static bool classof(const Atom *A) {
    // Something is a class of ConstAtom if it ISN'T a Var.
    return A->getKind() != A_Var;
  }
  void setExplicit(bool I) { IsExplicit = I; }
  bool isExplicit() const { return IsExplicit; }
  void setExplicitConstArray(bool I) { IsExplicitCA = I; }
  bool isExplicitConstArray() const { return IsExplicitCA; }
  virtual ConstAtom* reflectToTainted() {
    // By default, return self.
    // This method will be overridden in the derived classes that have tainted counterparts.
    return this;
  }

private:
    // This boolean is used to indicate that this is a const atom that
    // that was marked by explicit annotations from the developer and not
    // inferred by the 3C.
    bool IsExplicit = false;
    bool IsExplicitCA = false;
};

// This refers to a location that we are trying to solve for.
class VarAtom : public Atom {
  friend class Constraints;

public:
  enum VarKind { V_Param, V_Return, V_Other };

  VarAtom(uint32_t D) : Atom(A_Var), Loc(D), Name("q"), KindV(V_Other) {}
  VarAtom(uint32_t D, std::string N)
      : Atom(A_Var), Loc(D), Name(N), KindV(V_Other) {}
  VarAtom(uint32_t D, std::string N, VarKind V)
      : Atom(A_Var), Loc(D), Name(N), KindV(V) {}

  static bool classof(const Atom *S) { return S->getKind() == A_Var; }

  static std::string varKindToStr(VarKind V) {
    switch (V) {
    case V_Param:
      return ">>";
    case V_Return:
      return "<<";
    case V_Other:
      return "";
    }
    return "";
  }

  void print(llvm::raw_ostream &O) const override {
    O << varKindToStr(KindV) << Name << "_" << Loc;
  }

  void dump(void) const override { print(llvm::errs()); }

  void dumpJson(llvm::raw_ostream &O) const override {
    O << "\"" << varKindToStr(KindV) << Name << "_" << Loc << "\"";
  }

  void setDoNotTaint() {
      doNotTaint = true;
  }

  bool cannotTainted() {
      return doNotTaint;
  }

  void setIsAddress() {
       isAddress = true;
  }

  bool isAddr() {
      return isAddress;
  }

  bool operator==(const Atom &Other) const override {
    if (const VarAtom *V = llvm::dyn_cast<VarAtom>(&Other))
      return V->Loc == Loc;
    return false;
  }

  bool operator!=(const Atom &Other) const override {
    return !(*this == Other);
  }

  bool operator<(const Atom &Other) const override {
    if (const VarAtom *V = llvm::dyn_cast<VarAtom>(&Other))
      return Loc < V->Loc;
    return false;
  }

  uint32_t getLoc() const { return Loc; }
  std::string getName() const { return Name; }
  VarKind getVarKind() const { return KindV; }

  // Returns the constraints associated with this atom.
  std::set<Constraint *, PComp<Constraint *>> &getAllConstraints() {
    return Constraints;
  }

private:
  uint32_t Loc;
  std::string Name;
  const VarKind KindV;
  // The constraint expressions where this variable is mentioned on the
  // LHS of an equality.
  std::set<Constraint *, PComp<Constraint *>> Constraints;
  std::set<TaintedConstraint *, PComp<TaintedConstraint *>> TaintedConstraints;
  bool doNotTaint = false;
  bool isAddress = false;
};

/* ConstAtom ordering is:
    NTARR < ARR < PTR < WILD
   and all ConstAtoms < VarAtoms
*/

// This refers to the constant NTARR.
class TNTArrAtom : public ConstAtom {
public:
    TNTArrAtom() : ConstAtom(A_TNTArr) {}

    static bool classof(const Atom *S) { return S->getKind() == A_TNTArr; }

    void print(llvm::raw_ostream &O) const override { O << "TNTARR"; }

    void dump(void) const override { print(llvm::errs()); }

    void dumpJson(llvm::raw_ostream &O) const override { O << "\"TNTARR\""; }

    bool operator==(const Atom &Other) const override {
        return llvm::isa<TNTArrAtom>(&Other);
    }

    bool operator!=(const Atom &Other) const override {
        return !(*this == Other);
    }

    bool operator<(const Atom &Other) const override { return !(*this == Other); }
};

// This refers to the constant NTARR.
class NTArrAtom : public ConstAtom {
public:
  NTArrAtom() : ConstAtom(A_NTArr) {}

  static bool classof(const Atom *S) { return S->getKind() == A_NTArr; }

  void print(llvm::raw_ostream &O) const override { O << "NTARR"; }

  void dump(void) const override { print(llvm::errs()); }

  void dumpJson(llvm::raw_ostream &O) const override { O << "\"NTARR\""; }

  bool operator==(const Atom &Other) const override {
    return llvm::isa<NTArrAtom>(&Other);
  }

  bool operator!=(const Atom &Other) const override {
    return !(*this == Other);
  }

  bool operator<(const Atom &Other) const override { return !(*this == Other); }

  ConstAtom* reflectToTainted() override {
      return new TNTArrAtom();
  }
};

// This refers to the constant TARR.
class TaintedArrAtom : public ConstAtom {
public:
    TaintedArrAtom() : ConstAtom(A_TArr) {}

    static bool classof(const Atom *S) { return S->getKind() == A_TArr; }

    void print(llvm::raw_ostream &O) const override { O << "TARR"; }

    void dump(void) const override { print(llvm::errs()); }

    void dumpJson(llvm::raw_ostream &O) const override { O << "\"TARR\""; }

    bool operator==(const Atom &Other) const override {
        return llvm::isa<TaintedArrAtom>(&Other);
    }

    bool operator!=(const Atom &Other) const override {
        return !(*this == Other);
    }

    bool operator<(const Atom &Other) const override {
        return !(llvm::isa<TNTArrAtom>(&Other) || *this == Other);
    }
};

// This refers to the constant ARR.
class ArrAtom : public ConstAtom {
public:
  ArrAtom() : ConstAtom(A_Arr) {}

  static bool classof(const Atom *S) { return S->getKind() == A_Arr; }

  void print(llvm::raw_ostream &O) const override { O << "ARR"; }

  void dump(void) const override { print(llvm::errs()); }

  void dumpJson(llvm::raw_ostream &O) const override { O << "\"ARR\""; }

  bool operator==(const Atom &Other) const override {
    return llvm::isa<ArrAtom>(&Other);
  }

  bool operator!=(const Atom &Other) const override {
    return !(*this == Other);
  }

  bool operator<(const Atom &Other) const override {
    return !(llvm::isa<NTArrAtom>(&Other) || *this == Other);
  }

  ConstAtom* reflectToTainted() override {
      return new TaintedArrAtom();
  }
};

// This refers to the constant Tainted Pointer.
class TaintedPointerAtom : public ConstAtom {
public:
    TaintedPointerAtom() : ConstAtom(A_TPtr) {}

    static bool classof(const Atom *S) { return S->getKind() == A_TPtr; }

    void print(llvm::raw_ostream &O) const override { O << "TPTR"; }

    void dump(void) const override { print(llvm::errs()); }

    void dumpJson(llvm::raw_ostream &O) const override { O << "\"TPTR\""; }

    bool operator==(const Atom &Other) const override {
        return llvm::isa<TaintedPointerAtom>(&Other);
    }

    bool operator!=(const Atom &Other) const override {
        return !(*this == Other);
    }

    bool operator<(const Atom &Other) const override {
        return !(llvm::isa<TaintedArrAtom>(&Other) || llvm::isa<TNTArrAtom>(&Other) ||
                 *this == Other);
    }
};

class StructureAtom : public ConstAtom {
public:
    StructureAtom() : ConstAtom(A_struct) {}

    static bool classof(const Atom *S) { return S->getKind() == A_struct; }

    void print(llvm::raw_ostream &O) const override { O << StructName; }

    void dump(void) const override { print(llvm::errs()); }

    void dumpJson(llvm::raw_ostream &O) const override { O << StructName; }

    std::string getStructName() const {
        return StructName;
    }

    void setDecl(clang::RecordDecl *D) {
        VD = D;
    }

    clang::RecordDecl *getRecordDecl() {
        return VD;
    }

    void setStructName(std::string &Name) {
        StructName = Name;
    }

    void setASTContext(clang::ASTContext* C) {
        Ctx = C;
    }

    clang::ASTContext* getASTContext() {
        return Ctx;
    }

    bool operator==(const Atom &Other) const override {
        return llvm::isa<StructureAtom>(&Other);
    }

    bool operator!=(const Atom &Other) const override {
        return !(*this == Other);
    }

    bool operator<(const Atom &Other) const override {
        return !(llvm::isa<TaintedStructureAtom>(&Other) ||
                 *this == Other);
    }
    std::string StructName = "struct";
    clang::RecordDecl *VD = nullptr;
    clang::ASTContext* Ctx = nullptr;
};

class TaintedStructureAtom : public ConstAtom {
public:
    TaintedStructureAtom() : ConstAtom(A_Tstruct) {}

    static bool classof(const Atom *S) { return S->getKind() == A_Tstruct; }

    void print(llvm::raw_ostream &O) const override { O << StructName; }

    void dump(void) const override { print(llvm::errs()); }

    void dumpJson(llvm::raw_ostream &O) const override { O << StructName; }

    std::string getStructName() const {
        return StructName;
    }

    void setStructName(std::string &Name) {
        StructName = Name;
    }

    void setDecl(clang::VarDecl *D) {
        VD = D;
    }

    clang::VarDecl *getRecordDecl() {
        return VD;
    }

    bool operator==(const Atom &Other) const override {
        return llvm::isa<StructureAtom>(&Other);
    }

    bool operator!=(const Atom &Other) const override {
        return !(*this == Other);
    }

    bool operator<(const Atom &Other) const override {
        return !(*this == Other);
    }
    std::string StructName = "Tstruct";
    clang::VarDecl *VD = nullptr;
};

class DecoyTaintedStructureAtom : public ConstAtom {
public:
    DecoyTaintedStructureAtom() : ConstAtom(A_DTstruct) {}

    static bool classof(const Atom *S) { return S->getKind() == A_DTstruct; }

    void print(llvm::raw_ostream &O) const override { O << "_Decoy Tstruct"; }

    void dump(void) const override { print(llvm::errs()); }

    void dumpJson(llvm::raw_ostream &O) const override { O << "\"_Decoy Tstruct\""; }

    bool operator==(const Atom &Other) const override {
        return llvm::isa<StructureAtom>(&Other);
    }

    bool operator!=(const Atom &Other) const override {
        return !(*this == Other);
    }

    bool operator<(const Atom &Other) const override {
        return !(*this == Other);
    }
};

// This refers to the constant PTR.
class PtrAtom : public ConstAtom {
public:
  PtrAtom() : ConstAtom(A_Ptr) {}

  static bool classof(const Atom *S) { return S->getKind() == A_Ptr; }

  void print(llvm::raw_ostream &O) const override { O << "PTR"; }

  void dump(void) const override { print(llvm::errs()); }

  void dumpJson(llvm::raw_ostream &O) const override { O << "\"PTR\""; }

  bool operator==(const Atom &Other) const override {
    return llvm::isa<PtrAtom>(&Other);
  }

  bool operator!=(const Atom &Other) const override {
    return !(*this == Other);
  }

  bool operator<(const Atom &Other) const override {
    return !(llvm::isa<ArrAtom>(&Other) || llvm::isa<NTArrAtom>(&Other) ||
             *this == Other);
  }

  ConstAtom* reflectToTainted() override {
      return new TaintedPointerAtom();
  }
};

// This refers to the constant WILD.
class WildAtom : public ConstAtom {
public:
  WildAtom() : ConstAtom(A_Wild) {}

  static bool classof(const Atom *S) { return S->getKind() == A_Wild; }

  void print(llvm::raw_ostream &O) const override { O << "WILD"; }

  void dump(void) const override { print(llvm::errs()); }

  void dumpJson(llvm::raw_ostream &O) const override { O << "\"WILD\""; }

  bool operator==(const Atom &Other) const override {
    return llvm::isa<WildAtom>(&Other);
  }

  bool operator!=(const Atom &Other) const override {
    return !(*this == Other);
  }

  bool operator<(const Atom &Other) const override {
    return !(llvm::isa<ArrAtom>(&Other) || llvm::isa<NTArrAtom>(&Other) ||
             llvm::isa<PtrAtom>(&Other) || *this == Other);
  }

  ConstAtom* reflectToTainted() override {
        return new TaintedPointerAtom();
  }
};

// Helper struct requiring locations for every reason, that is,
// Constraints need reasons and locations to provide good user feedback
struct ReasonLoc {
  ReasonLoc() : Reason(DEFAULT_REASON), Location(PersistentSourceLoc()) {}
  ReasonLoc(std::string Reason, PersistentSourceLoc Loc)
      : Reason(Reason), Location(Loc) {}
  bool isDefault() const { return Reason == DEFAULT_REASON; }
  std::string Reason;
  PersistentSourceLoc Location;
};

// Represents constraints of the form:
//  - a >= b
class Constraint {
public:
  enum ConstraintKind { C_Geq };

private:
  const ConstraintKind Kind;
  ReasonLoc Reason;
  std::vector<ReasonLoc> ExtraReasons;

public:
  Constraint(ConstraintKind K) : Kind(K) {}
  Constraint(ConstraintKind K, const ReasonLoc &Rsn) : Kind(K), Reason(Rsn) {}

  virtual ~Constraint() {}

  ConstraintKind getKind() const { return Kind; }

  virtual void print(llvm::raw_ostream &) const = 0;
  virtual void dump(void) const = 0;
  virtual void dumpJson(llvm::raw_ostream &) const = 0;
  virtual bool operator==(const Constraint &Other) const = 0;
  virtual bool operator!=(const Constraint &Other) const = 0;
  virtual bool operator<(const Constraint &Other) const = 0;
  virtual std::string getReasonText() const { return Reason.Reason; }
  virtual const ReasonLoc &getReason() const { return Reason; }
  // Alter the internal reason and remove any additional reasons
  virtual void setReason(const ReasonLoc &Rsn) {
    Reason = Rsn;
    ExtraReasons.clear();
  }
  virtual std::vector<ReasonLoc> &additionalReasons() { return ExtraReasons; }
  // include additional reasons that will appear in output as notes
  virtual void addReason(const ReasonLoc &Rsn) {
    ExtraReasons.push_back(Rsn);
  }

  bool isUnwritable(void) const {
    return getReasonText() == UNWRITABLE_REASON;
  }

  const PersistentSourceLoc &getLocation() const { return Reason.Location; }
};

// a >= b
class Geq : public Constraint {
  friend class VarAtom;

public:
  Geq(Atom *Lhs, Atom *Rhs, const ReasonLoc &Rsn, bool IsCC = true,
      bool Soft = false)
      : Constraint(C_Geq, Rsn), Lhs(Lhs), Rhs(Rhs), IsCheckedConstraint(IsCC),
        IsSoft(Soft) {}

  static bool classof(const Constraint *C) { return C->getKind() == C_Geq; }

  void print(llvm::raw_ostream &O) const override {
    Lhs->print(O);
    std::string Kind = IsCheckedConstraint ? " (C)>= " : " (P)>= ";
    O << Kind;
    Rhs->print(O);
    O << ", Reason: " << getReasonText();
  }

  void dump(void) const override { print(llvm::errs()); }

  void dumpJson(llvm::raw_ostream &O) const override {
    O << "{\"Geq\":{\"Atom1\":";
    Lhs->dumpJson(O);
    O << ", \"Atom2\":";
    Rhs->dumpJson(O);
    O << ", \"isChecked\":";
    O << (IsCheckedConstraint ? "true" : "false");
    O << ", \"Reason\":";
    llvm::json::Value ReasonVal(getReasonText());
    O << ReasonVal;
    O << "}}";
  }

  Atom *getLHS(void) const { return Lhs; }
  Atom *getRHS(void) const { return Rhs; }

  void setChecked(ConstAtom *C) {
    assert(!IsCheckedConstraint);
    IsCheckedConstraint = true;
    if (llvm::isa<ConstAtom>(Lhs)) {
      Lhs = Rhs; // reverse direction for checked constraint
      Rhs = C;
    } else {
      assert(llvm::isa<ConstAtom>(Rhs));
      Rhs = C;
    }
  }

  bool constraintIsChecked(void) const { return IsCheckedConstraint; }

  bool operator==(const Constraint &Other) const override {
    if (const Geq *E = llvm::dyn_cast<Geq>(&Other))
      return *Lhs == *E->Lhs && *Rhs == *E->Rhs &&
             IsCheckedConstraint == E->IsCheckedConstraint;
    return false;
  }

  bool operator!=(const Constraint &Other) const override {
    return !(*this == Other);
  }

  bool operator<(const Constraint &Other) const override {
    ConstraintKind K = Other.getKind();
    if (K == C_Geq) {
      const Geq *E = llvm::dyn_cast<Geq>(&Other);
      assert(E != nullptr);
      if (*Lhs == *E->Lhs) {
        if (*Rhs == *E->Rhs) {
          if (IsCheckedConstraint == E->IsCheckedConstraint)
            return false;
          return IsCheckedConstraint < E->IsCheckedConstraint;
        }
        return *Rhs < *E->Rhs;
      }
      return *Lhs < *E->Lhs;
    }
    return C_Geq < K;
  }

  bool isSoft(void) { return IsSoft; }

private:
  Atom *Lhs;
  Atom *Rhs;
  bool IsCheckedConstraint;
  bool IsSoft;
};

class TaintedConstraint {
public:
    enum ConstraintKind { C_Geq };

private:
    const ConstraintKind Kind;
    ReasonLoc Reason;
    std::vector<ReasonLoc> ExtraReasons;

public:
    TaintedConstraint(ConstraintKind K) : Kind(K) {}
    TaintedConstraint(ConstraintKind K, const ReasonLoc &Rsn) : Kind(K), Reason(Rsn) {}

    virtual ~TaintedConstraint() {}

    ConstraintKind getKind() const { return Kind; }

    virtual void print(llvm::raw_ostream &) const = 0;
    virtual void dump(void) const = 0;
    virtual void dumpJson(llvm::raw_ostream &) const = 0;
    virtual bool operator==(const TaintedConstraint &Other) const = 0;
    virtual bool operator!=(const TaintedConstraint &Other) const = 0;
    virtual bool operator<(const TaintedConstraint &Other) const = 0;
    virtual std::string getReasonText() const { return Reason.Reason; }
    virtual const ReasonLoc &getReason() const { return Reason; }
    // Alter the internal reason and remove any additional reasons
    virtual void setReason(const ReasonLoc &Rsn) {
        Reason = Rsn;
        ExtraReasons.clear();
    }
    virtual std::vector<ReasonLoc> &additionalReasons() { return ExtraReasons; }
    // include additional reasons that will appear in output as notes
    virtual void addReason(const ReasonLoc &Rsn) {
        ExtraReasons.push_back(Rsn);
    }

    bool isUnwritable(void) const {
        return getReasonText() == UNWRITABLE_REASON;
    }

    const PersistentSourceLoc &getLocation() const { return Reason.Location; }
};

// a >= b {For tainted pointers}
class TGeq : public TaintedConstraint {
    friend class VarAtom;

public:
    TGeq(Atom *Lhs, Atom *Rhs, const ReasonLoc &Rsn, bool IsT = true,
        bool Soft = false)
            : TaintedConstraint(C_Geq, Rsn), Lhs(Lhs), Rhs(Rhs), IsTaintedConstraint(IsT),
              IsSoft(Soft) {}

    static bool classof(const TaintedConstraint *C) { return C->getKind() == C_Geq; }

    void print(llvm::raw_ostream &O) const override {
        Lhs->print(O);
        std::string Kind = IsTaintedConstraint ? " (T)>= " : " (P)>= ";
        O << Kind;
        Rhs->print(O);
        O << ", Reason: " << getReasonText();
    }

    void dump(void) const override { print(llvm::errs()); }

    void dumpJson(llvm::raw_ostream &O) const override {
        O << "{\"Geq\":{\"Atom1\":";
        Lhs->dumpJson(O);
        O << ", \"Atom2\":";
        Rhs->dumpJson(O);
        O << ", \"isTainted\":";
        O << (IsTaintedConstraint ? "true" : "false");
        O << ", \"Reason\":";
        llvm::json::Value ReasonVal(getReasonText());
        O << ReasonVal;
        O << "}}";
    }

    Atom *getLHS(void) const { return Lhs; }
    Atom *getRHS(void) const { return Rhs; }

    void setTainted(ConstAtom *C) {
        assert(!IsTaintedConstraint);
        IsTaintedConstraint = true;
        if (llvm::isa<ConstAtom>(Lhs)) {
            Lhs = Rhs; // reverse direction for checked constraint
            Rhs = C;
        } else {
            assert(llvm::isa<ConstAtom>(Rhs));
            Rhs = C;
        }
    }

    bool constraintIsTainted(void) const { return IsTaintedConstraint; }

    bool operator==(const TaintedConstraint &Other) const override {
        if (const TGeq *E = llvm::dyn_cast<TGeq>(&Other))
            return *Lhs == *E->Lhs && *Rhs == *E->Rhs &&
                    IsTaintedConstraint == E->IsTaintedConstraint;
        return false;
    }

    bool operator!=(const TaintedConstraint &Other) const override {
        return !(*this == Other);
    }

    bool operator<(const TaintedConstraint &Other) const override {
        ConstraintKind K = Other.getKind();
        if (K == C_Geq) {
            const TGeq *E = llvm::dyn_cast<TGeq>(&Other);
            assert(E != nullptr);
            if (*Lhs == *E->Lhs) {
                if (*Rhs == *E->Rhs) {
                    if (IsTaintedConstraint == E->IsTaintedConstraint)
                        return false;
                    return IsTaintedConstraint < E->IsTaintedConstraint;
                }
                return *Rhs < *E->Rhs;
            }
            return *Lhs < *E->Lhs;
        }
        return C_Geq < K;
    }

    bool isSoft(void) { return IsSoft; }

private:
    Atom *Lhs;
    Atom *Rhs;
    bool IsTaintedConstraint;
    bool IsSoft;
};

// This is the solution, the first item is Checked Solution and the second
// is Ptr solution
typedef std::pair<ConstAtom *, ConstAtom *> VarSolTy;
typedef std::map<VarAtom *, VarSolTy, PComp<VarAtom *>> EnvironmentMap;
typedef std::pair<ConstAtom *, ConstAtom *> TaintedVarSolTy;
typedef std::map<VarAtom *, TaintedVarSolTy, PComp<VarAtom *>> TaintedEnvironmentMap;

typedef uint32_t ConstraintKey;

using VarAtomPred = llvm::function_ref<bool(VarAtom *)>;

class ConstraintsEnv {

public:
  ConstraintsEnv() : ConsFreeKey(0), UseChecked(true) { Environment.clear(); }
  const EnvironmentMap &getVariables() const { return Environment; }
  void dump() const;
  void print(llvm::raw_ostream &) const;
  void dumpJson(llvm::raw_ostream &) const;
  /* constraint variable generation */
  VarAtom *getFreshVar(VarSolTy InitC, std::string Name, VarAtom::VarKind VK);
  VarAtom *getOrCreateVar(ConstraintKey V, VarSolTy InitC, std::string Name,
                          VarAtom::VarKind VK);

  VarAtom *getTaintedFreshVar(TaintedVarSolTy InitC, std::string Name, VarAtom::VarKind VK);
  VarAtom *getOrCreateTaintedVar(ConstraintKey V, TaintedVarSolTy InitC, std::string Name,
                        VarAtom::VarKind VK);
VarAtom *getTaintedStructFreshVar(TaintedVarSolTy InitC, std::string Name, VarAtom::VarKind VK);
VarAtom *getOrCreateTaintedStructVar(ConstraintKey V, TaintedVarSolTy InitC, std::string Name,
                               VarAtom::VarKind VK);
  VarAtom *getVar(ConstraintKey V) const;
  /* solving */
  ConstAtom *getAssignment(Atom *A);
  bool assign(VarAtom *V, ConstAtom *C);
  void doCheckedSolve(bool DoC) { UseChecked = DoC; }
  void mergePtrTypes();
  std::set<VarAtom *> resetSolution(VarAtomPred Pred, ConstAtom *CA);
  void resetFullSolution(VarSolTy InitC, TaintedVarSolTy InitT);
  bool checkAssignment(VarSolTy C);
  bool checkTaintedAssignment(TaintedVarSolTy C);
  std::set<VarAtom *> filterAtoms(VarAtomPred Pred);

  VarAtom *getFreshStructVar(TaintedVarSolTy InitV, std::string Name, VarAtom::VarKind VK);

  InfluenceMap SolPtrTypCG_InfluenceMap;
  InfluenceRootMap SolPtrTypCG_Root_InfluenceMap;
  InfluenceMap SolChkCG_InfluenceMap;

private:
  EnvironmentMap Environment; // Solution map: Var --> Sol
    //  TaintedEnvironmentMap TaintedEnvironment; // Solution map: TaintedVar --> TaintedSol
  uint32_t ConsFreeKey;       // Next available integer to assign to a Var
  bool UseChecked;            // Which solution map to use -- checked (vs. ptyp)
  bool assignTainted(VarAtom *V, ConstAtom *C);
  VarAtom *getTaintedVar(ConstraintKey V) const;

    VarAtom *getFreshStructVar(ConstraintKey V, TaintedVarSolTy InitC, std::string Name, VarAtom::VarKind VK);

};

class ConstraintVariable;

class Constraints {
public:
  Constraints();
  ~Constraints();
  // Remove the copy constructor, so we never accidentally copy this thing.
  Constraints(const Constraints &O) = delete;

  typedef std::set<Constraint *, PComp<Constraint *>> ConstraintSet;
  typedef std::set<TaintedConstraint *, PComp<TaintedConstraint *>> TaintedConstraintSet;
  // Map from a unique key of a function to its constraint variables.
  typedef std::map<std::string, std::set<ConstraintVariable *>>
      FuncKeyToConsMap;

  bool removeConstraint(Constraint *C);
  bool addConstraint(Constraint *C);
  // It's important to return these by reference. Programs can have
  // 10-100-100000 constraints and variables, and copying them each time
  // a client wants to examine the environment is untenable.
  const ConstraintSet &getConstraints() const { return TheConstraints; }
  const EnvironmentMap &getVariables() const {
    return Environment.getVariables();
  }

  void editConstraintHook(Constraint *C);

  void solve();
  void dump() const;
  void print(llvm::raw_ostream &) const;
  void dumpJson(llvm::raw_ostream &) const;

  Geq *createGeq(Atom *Lhs, Atom *Rhs, ReasonLoc Rsn,
                 bool IsCheckedConstraint = true, bool Soft = false);

  TGeq *createTaintedGeq(Atom *Lhs, Atom *Rhs, ReasonLoc Rsn,
                 bool IsTaintedConstraint = true, bool Soft = false);

  VarAtom *createFreshGEQ(std::string Name, VarAtom::VarKind VK, ConstAtom *Con,
                          ReasonLoc Rsn = ReasonLoc());

  VarAtom *createFreshTGEQ(std::string Name, VarAtom::VarKind VK, ConstAtom *Con,
                            ReasonLoc Rsn = ReasonLoc());

  VarAtom *getOrCreateVar(ConstraintKey V, std::string Name,
                          VarAtom::VarKind VK);
  VarAtom *getVar(ConstraintKey V) const;
  PtrAtom *getPtr() const;
  TaintedPointerAtom *getTaintedPtr() const;
  StructureAtom *getStruct() const;
  TaintedStructureAtom *getTaintedStruct() const;
  ArrAtom *getArr() const;
  TaintedArrAtom *getTaintedArr() const;
  NTArrAtom *getNTArr() const;
  TNTArrAtom *getTaintedNTArr() const;
  WildAtom *getWild() const;
  ConstAtom *getAssignment(Atom *A);
  const ConstraintsGraph &getChkCG() const;
  const ConstraintsGraph &getPtrTypCG() const;

  void resetEnvironment();
  bool checkInitialEnvSanity();

  // Remove all constraints that were generated because of the
  // provided reason.
  bool removeAllConstraintsOnReason(std::string &Reason,
                                    ConstraintSet &RemovedCons);

    VarAtom *getFreshTaintedVar(std::string Name, VarAtom::VarKind VK);

    VarAtom *getFreshVar(std::string Name, VarAtom::VarKind VK);

    TNTArrAtom *getTNTArr() const;

    bool addTaintedConstraint(TaintedConstraint *C);

    VarAtom *getFreshStructVar(std::string Name, VarAtom::VarKind VK);

    void solve(ProgramInfo &Info);

    void printISTM(llvm::raw_ostream &O, EnvironmentMap Environment, std::string python_script) const;


private:
  ConstraintSet TheConstraints;
  //TaintedConstraintSet TheTaintedConstraints;
  // These are constraint graph representation of constraints.
  ConstraintsGraph *ChkCG;
  //ConstraintsGraph *TaintedCG;
  ConstraintsGraph *PtrTypCG;
  std::map<std::string, ConstraintSet> ConstraintsByReason;
//  std::map<std::string, TaintedConstraintSet> TaintedConstraintsByReason;
  ConstraintsEnv Environment;

  // Managing constraints based on the underlying reason.
  // add constraint to the map.
  bool addReasonBasedConstraint(Constraint *C);
  // Remove constraint from the map.
  bool removeReasonBasedConstraint(Constraint *C);

  VarSolTy getDefaultSolution();
  TaintedVarSolTy getDefaultTaintedSolution();

  // Solve constraint set via graph-based dynamic transitive closure
  bool graphBasedSolve();

  // These atoms can be singletons, so we'll store them in the
  // Constraints class.
  PtrAtom *PrebuiltPtr;
  ArrAtom *PrebuiltArr;
  TaintedArrAtom* PrebuiltTaintedArr;
  NTArrAtom *PrebuiltNTArr;
  TNTArrAtom *PrebuiltTNTArr;
  WildAtom *PrebuiltWild;
  TaintedPointerAtom *PrebuiltTainted;
  StructureAtom *PrebuiltStruct;
  TaintedStructureAtom *PrebuiltTaintedStruct;

  VarAtom *getOrCreateTaintedVar(ConstraintKey V, std::string Name, VarAtom::VarKind VK);

    void editTaintedConstraintHook(TaintedConstraint *C);

    bool addReasonBasedTaintedConstraint(TaintedConstraint *C);

    bool removeReasonBasedConstraint(TaintedConstraint *C);

    TaintedVarSolTy getDefaultTaintedStructureSolution();

    VarSolTy getDefaultStructureSolution();

    bool graphBasedSolve(ProgramInfo &Info);


    void calculateInfluence(VarAtom *Current, std::set<VarAtom *> &Visited, std::map<VarAtom *, double> &Scores,
                            InfluenceRootMap &influence_root_map, ConstraintsGraph &CG, ProgramInfo *Info);

    void assignInfluenceScores(ConstraintsGraph &CG, std::map<VarAtom *, double> &InfluenceScores,
                               InfluenceRootMap &Influence_root_map, ProgramInfo *Info);

};

#endif
