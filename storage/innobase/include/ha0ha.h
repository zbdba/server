/*****************************************************************************

Copyright (c) 1994, 2016, Oracle and/or its affiliates. All Rights Reserved.
Copyright (c) 2018, 2020, MariaDB Corporation.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1335 USA

*****************************************************************************/

/**************************************************//**
@file include/ha0ha.h
The hash table interface for the adaptive hash index

Created 8/18/1994 Heikki Tuuri
*******************************************************/

#ifndef ha0ha_h
#define ha0ha_h

#include "hash0hash.h"
#include "page0types.h"
#include "buf0types.h"
#include "rem0types.h"

#ifdef BTR_CUR_HASH_ADAPT
/*************************************************************//**
Looks for an element in a hash table.
@return pointer to the data of the first hash table node in chain
having the fold number, NULL if not found */
UNIV_INLINE
const rec_t*
ha_search_and_get_data(
/*===================*/
	hash_table_t*	table,	/*!< in: hash table */
	ulint		fold);	/*!< in: folded value of the searched data */
/*********************************************************//**
Looks for an element when we know the pointer to the data and updates
the pointer to data if found.
@return TRUE if found */
ibool
ha_search_and_update_if_found_func(
/*===============================*/
	hash_table_t*	table,	/*!< in/out: hash table */
	ulint		fold,	/*!< in: folded value of the searched data */
	const rec_t*	data,	/*!< in: pointer to the data */
#if defined UNIV_AHI_DEBUG || defined UNIV_DEBUG
	buf_block_t*	new_block,/*!< in: block containing new_data */
#endif /* UNIV_AHI_DEBUG || UNIV_DEBUG */
	const rec_t*	new_data);/*!< in: new pointer to the data */

#if defined UNIV_AHI_DEBUG || defined UNIV_DEBUG
/** Looks for an element when we know the pointer to the data and
updates the pointer to data if found.
@param table in/out: hash table
@param fold in: folded value of the searched data
@param data in: pointer to the data
@param new_block in: block containing new_data
@param new_data in: new pointer to the data */
# define ha_search_and_update_if_found(table,fold,data,new_block,new_data) \
	ha_search_and_update_if_found_func(table,fold,data,new_block,new_data)
#else /* UNIV_AHI_DEBUG || UNIV_DEBUG */
/** Looks for an element when we know the pointer to the data and
updates the pointer to data if found.
@param table in/out: hash table
@param fold in: folded value of the searched data
@param data in: pointer to the data
@param new_block ignored: block containing new_data
@param new_data in: new pointer to the data */
# define ha_search_and_update_if_found(table,fold,data,new_block,new_data) \
	ha_search_and_update_if_found_func(table,fold,data,new_data)
#endif /* UNIV_AHI_DEBUG || UNIV_DEBUG */
#endif /* BTR_CUR_HASH_ADAPT */

#ifdef BTR_CUR_HASH_ADAPT
/*************************************************************//**
Inserts an entry into a hash table. If an entry with the same fold number
is found, its node is updated to point to the new data, and no new node
is inserted.
@return TRUE if succeed, FALSE if no more memory could be allocated */
ibool
ha_insert_for_fold_func(
/*====================*/
	hash_table_t*	table,	/*!< in: hash table */
	ulint		fold,	/*!< in: folded value of data; if a node with
				the same fold value already exists, it is
				updated to point to the same data, and no new
				node is created! */
#if defined UNIV_AHI_DEBUG || defined UNIV_DEBUG
	buf_block_t*	block,	/*!< in: buffer block containing the data */
#endif /* UNIV_AHI_DEBUG || UNIV_DEBUG */
	const rec_t*	data);	/*!< in: data, must not be NULL */

#if defined UNIV_AHI_DEBUG || defined UNIV_DEBUG
/**
Inserts an entry into a hash table. If an entry with the same fold number
is found, its node is updated to point to the new data, and no new node
is inserted.
@return TRUE if succeed, FALSE if no more memory could be allocated
@param t in: hash table
@param f in: folded value of data
@param b in: buffer block containing the data
@param d in: data, must not be NULL */
# define ha_insert_for_fold(t,f,b,d) 	do {		\
	ha_insert_for_fold_func(t,f,b,d);		\
	MONITOR_INC(MONITOR_ADAPTIVE_HASH_ROW_ADDED);	\
} while(0)
#else /* UNIV_AHI_DEBUG || UNIV_DEBUG */
/**
Inserts an entry into a hash table. If an entry with the same fold number
is found, its node is updated to point to the new data, and no new node
is inserted.
@return TRUE if succeed, FALSE if no more memory could be allocated
@param t in: hash table
@param f in: folded value of data
@param b ignored: buffer block containing the data
@param d in: data, must not be NULL */
# define ha_insert_for_fold(t,f,b,d)	do {		\
	ha_insert_for_fold_func(t,f,d);			\
	MONITOR_INC(MONITOR_ADAPTIVE_HASH_ROW_ADDED);	\
} while (0)
#endif /* UNIV_AHI_DEBUG || UNIV_DEBUG */

/*********************************************************//**
Looks for an element when we know the pointer to the data and deletes
it from the hash table if found.
@return TRUE if found */
UNIV_INLINE
ibool
ha_search_and_delete_if_found(
/*==========================*/
	hash_table_t*	table,	/*!< in: hash table */
	ulint		fold,	/*!< in: folded value of the searched data */
	const rec_t*	data);	/*!< in: pointer to the data */

/*****************************************************************//**
Removes from the chain determined by fold all nodes whose data pointer
points to the page given. */
void
ha_remove_all_nodes_to_page(
/*========================*/
	hash_table_t*	table,	/*!< in: hash table */
	ulint		fold,	/*!< in: fold value */
	const page_t*	page);	/*!< in: buffer page */
#if defined UNIV_AHI_DEBUG || defined UNIV_DEBUG
/*************************************************************//**
Validates a given range of the cells in hash table.
@return TRUE if ok */
ibool
ha_validate(
/*========*/
	hash_table_t*	table,		/*!< in: hash table */
	ulint		start_index,	/*!< in: start index */
	ulint		end_index);	/*!< in: end index */
#endif /* defined UNIV_AHI_DEBUG || defined UNIV_DEBUG */

/** The hash table external chain node */
struct ha_node_t {
	ulint		fold;	/*!< fold value for the data */
	ha_node_t*	next;	/*!< next chain node or NULL if none */
#if defined UNIV_AHI_DEBUG || defined UNIV_DEBUG
	buf_block_t*	block;	/*!< buffer block containing the data, or NULL */
#endif /* UNIV_AHI_DEBUG || UNIV_DEBUG */
	const rec_t*	data;	/*!< pointer to the data */
};

#include "ha0ha.ic"
#endif /* BTR_CUR_HASH_ADAPT */

#endif
