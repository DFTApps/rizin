// SPDX-FileCopyrightText: 2024 z3phyr <giridh1337@gmail.com>
// SPDX-License-Identifier: LGPL-3.0-only

#ifndef RZ_ROP_H
#define RZ_ROP_H

/**
 * \file rz_rop.h
 * \brief Return-Oriented Programming (ROP) related APIs and structures..
 *
 * This file contains definitions, structures, and function prototypes for handling ROP gadgets and constraints.
 */

#include <rz_cmd.h>
#include <rz_il.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Information about a register.
 */
typedef struct rz_rop_reg_info_t {
	char *name;
	bool is_mem_read; ///< Register involved in Memory read.
	bool is_pc_write; ///< PC write flag.
	bool is_var_read; ///< Register involved in Variable read.
	bool is_var_write; ///< Register involved in Variable write.
	bool is_mem_write; ///< Register involved in Memory write.
	ut64 init_val;
	ut64 new_val;
	ut64 bits; ///< Register bits for capturing cast
	RzILOpPure *value_transformations; ///< TODO: Captures Value transformations.
} RzRopRegInfo;

/**
 * \brief Information about a ROP gadget.
 */
typedef struct rz_rop_gadget_info_t {
	ut64 address; ///< Gadget address.
	ut64 stack_change; ///< Stack change.
	ut64 curr_pc_val; ///< Current PC value.
	bool is_pc_write; ///< PC write flag.
	bool is_syscall; ///< Syscall flag.
	HtUP /* <RzAnalysisOp> */ *analysis_cache; ///< Maps hitlist address to its respective \p RzAnalysisOp.
	RzPVector /*<RzRopRegInfo *>*/ *modified_registers; ///< Modified registers.
	RzList /*<RzRopRegInfo *>*/ *dependencies; ///< Dependencies.
} RzRopGadgetInfo;

/**
 * \brief Types of IL instructions for ROP constraints.
 */
typedef enum rz_rop_il_instr_type {
	MOV_CONST, ///< reg <- const
	MOV_REG, ///< reg <- reg
	MOV_OP_CONST, ///< reg <- reg OP const
	MOV_OP_REG, ///< reg <- reg OP reg
	SYSCALL, ///< syscall
} RzRopILInstructionType;

/**
 * \brief Argument types for ROP constraints.
 */
typedef enum {
	SRC_REG,
	DST_REG,
	SRC_CONST,
	DST_REG_SECOND,
	OP,
	NUM_ARGS
} RzRopArgType;

/**
 * \brief ROP request mask for filtering gadgets.
 */
typedef enum {
	RZ_ROP_GADGET_PRINT = 1 << 0, ///< Print ROP gadgets.
	RZ_ROP_GADGET_PRINT_DETAIL = 1 << 1, ///< Detailed ROP gadgets.
	RZ_ROP_GADGET_ANALYZE = 1 << 2, ///< Detailed ROP gadgets.
	RZ_ROP_GADGET_ALL = RZ_ROP_GADGET_PRINT | RZ_ROP_GADGET_PRINT_DETAIL | RZ_ROP_GADGET_ANALYZE ///< All ROP gadgets requests.
} RzRopRequestMask;

/**
 * \brief Pair representing an end gadget with instruction offset and delay size.
 */
typedef struct rz_rop_endlist_pair_t {
	int instr_offset; ///< Instruction offset.
	int delay_size; ///< Delay size.
} RzRopEndListPair;

/**
 * \brief Structure representing a ROP constraint.
 */
typedef struct rz_rop_constraint_t {
	RzRopILInstructionType type; ///< IL instruction type.
	char *args[NUM_ARGS]; ///< Arguments.
} RzRopConstraint;

/**
 * \brief Structure representing a ROP search context.
 */
typedef struct rz_rop_search_context_t {
	ut8 max_instr;
	ut8 subchain;
	ut8 crop;
	char *greparg;
	const char *mode_str;
	const char *arch;
	bool regexp;
	bool cache;
	RzRopRequestMask mask;
	RzCmdStateOutput *state;
	int increment;
	int max_count;
	ut64 from;
	ut64 to;
	RzList /*<RzRopEndListPair *>*/ *end_list;
	HtSU *unique_hitlists;
} RzRopSearchContext;

/**
 * \brief Enum for different ROP register events.
 */
typedef enum {
	RZ_ROP_EVENT_VAR_READ,
	RZ_ROP_EVENT_VAR_WRITE,
	RZ_ROP_EVENT_MEM_READ,
	RZ_ROP_EVENT_MEM_WRITE,
	RZ_ROP_EVENT_PC_WRITE,
	RZ_ROP_EVENT_COUNT // This should always be the last element
} RzRopEvent;

/**
 * \brief Function pointer type for event check functions.
 */
typedef bool (*rz_rop_event_check_fn)(const RzRopRegInfo *);

/**
 * \brief Array of event check functions.
 */
extern rz_rop_event_check_fn rz_rop_event_functions[RZ_ROP_EVENT_COUNT];

// Command APIs
RZ_API RzCmdStatus rz_core_rop_search(RZ_NONNULL RzCore *core, RZ_NONNULL RZ_OWN RzRopSearchContext *context);
RZ_API RzCmdStatus rz_core_rop_gadget_info(RZ_NONNULL RzCore *core, RZ_NONNULL RZ_OWN RzRopSearchContext *context);
RZ_API bool rz_core_rop_analyze_constraint(const RZ_NONNULL RzCore *core, const RZ_NONNULL char *str,
	RZ_NONNULL RZ_OUT RzRopConstraint *rop_constraint);

// ROP Search Context APIs
RZ_API RZ_OWN RzRopSearchContext *rz_core_rop_search_context_new(RZ_NONNULL const RzCore *core, RZ_NULLABLE const char *greparg, bool regexp,
	const RzRopRequestMask mask, RZ_NONNULL RZ_BORROW RzCmdStateOutput *state);
RZ_API void rz_core_rop_search_context_free(RZ_NULLABLE RzRopSearchContext *context);

// ROP Constraint APIs
RZ_API void rz_core_rop_constraint_free(RZ_NULLABLE void *data);
RZ_API RZ_OWN RzPVector /*<RzRopConstraint *>*/ *rz_core_rop_constraint_map_new(void);

// ROP Gadget Info APIs
RZ_API void rz_core_rop_gadget_info_free(RZ_NULLABLE RzRopGadgetInfo *gadget_info);
RZ_API void rz_core_rop_gadget_info_add_register(const RZ_NONNULL RZ_OUT RzRopGadgetInfo *gadget_info,
	RZ_NONNULL RzRopRegInfo *reg_info, bool is_dependency);
RZ_API bool rz_core_rop_gadget_info_update_register(const RZ_INOUT RzRopGadgetInfo *gadget_info, RZ_INOUT RZ_NONNULL RzRopRegInfo *new_reg_info);
RZ_API RZ_OWN RzRopGadgetInfo *rz_core_rop_gadget_info_new(ut64 address);
RZ_API RZ_OWN RzRopRegInfo *rz_core_rop_reg_info_dup(RZ_BORROW RZ_NONNULL RzRopRegInfo *src);
RZ_API void rz_core_rop_reg_info_free(RZ_NULLABLE RzRopRegInfo *reg_info);
RZ_API RZ_OWN RzRopRegInfo *rz_core_rop_reg_info_new(RZ_NONNULL const RzCore *core, RZ_NONNULL const RzILEvent *evt,
	ut64 init_val, ut64 new_val);
RZ_API RZ_BORROW RzRopRegInfo *rz_core_rop_gadget_info_get_modified_register(const RZ_NONNULL RzRopGadgetInfo *gadget_info,
	const RZ_NONNULL char *name);
RZ_API bool rz_core_rop_gadget_info_has_register(const RZ_NONNULL RzRopGadgetInfo *gadget_info, const RZ_NONNULL char *name);
RZ_API RZ_OWN RzPVector /*<RzRopRegInfo *>*/ *rz_core_rop_gadget_get_reg_info_by_event(const RZ_NONNULL RzRopGadgetInfo *gadget_info, RzRopEvent event);
RZ_API RZ_OWN RzPVector /*<RzRopRegInfo *>*/ *rz_core_rop_get_reg_info_by_reg_names(const RZ_NONNULL RzRopGadgetInfo *gadget_info, RZ_NONNULL const RzPVector /*<char *>*/ *registers);
RZ_API bool rz_core_rop_gadget_reg_info_has_event(const RZ_NONNULL RzRopGadgetInfo *gadget_info,
	RzRopEvent event, const RZ_NULLABLE char *reg_name);

#ifdef __cplusplus
}
#endif
#endif // RZ_ROP_H
