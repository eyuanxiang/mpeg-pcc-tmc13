/* The copyright in this software is being made available under the BSD
 * Licence, included below.  This software may be subject to other third
 * party and contributor rights, including patent rights, and no such
 * rights are granted under this licence.
 *
 * Copyright (c) 2017-2018, ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * * Neither the name of the ISO/IEC nor the names of its contributors
 *   may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "PCCMath.h"

#include <cstdint>
#include <ostream>
#include <vector>

namespace pcc {

//============================================================================

enum class PayloadType
{
  kSequenceParameterSet = 0,
  kGeometryParameterSet = 1,
  kGeometryBrick = 2,
  kAttributeParameterSet = 3,
  kAttributeBrick = 4,
  kTileInventory = 5,
  kFrameBoundaryMarker = 6,
};

//============================================================================

enum class KnownAttributeLabel : uint32_t
{
  kColour = 0,
  kReflectance = 1,
};

//============================================================================

struct AttributeLabel {
  uint32_t attribute_label_four_bytes = 0xffffffffu;

  //--------------------------------------------------------------------------

  AttributeLabel() = default;

  AttributeLabel(KnownAttributeLabel known_attribute_label)
  {
    attribute_label_four_bytes = int(known_attribute_label);
  }

  //--------------------------------------------------------------------------

  friend bool
  operator==(const AttributeLabel& lhs, const KnownAttributeLabel& rhs)
  {
    return uint32_t(rhs) == lhs.attribute_label_four_bytes;
  }

  //--------------------------------------------------------------------------

  friend bool
  operator!=(const AttributeLabel& lhs, const KnownAttributeLabel& rhs)
  {
    return !(lhs == rhs);
  }

  //--------------------------------------------------------------------------

  bool known_attribute_label_flag() const
  {
    switch (KnownAttributeLabel(attribute_label_four_bytes)) {
    case KnownAttributeLabel::kColour:
    case KnownAttributeLabel::kReflectance: return true;
    }

    return false;
  }

  //--------------------------------------------------------------------------

  KnownAttributeLabel known_attribute_label() const
  {
    return KnownAttributeLabel(attribute_label_four_bytes);
  }
};

//============================================================================

std::ostream& operator<<(std::ostream& os, const AttributeLabel& label);

//============================================================================

enum class AttributeEncoding
{
  kPredictingTransform = 0,
  kRAHTransform = 1,
  kLiftingTransform = 2,
};

//============================================================================

enum class AxisOrder
{
  kZYX = 0,
  kXYZ = 1,
  kXZY = 2,
  kYZX = 3,
  kZYX_4 = 4,
  kZXY = 5,
  kYXZ = 6,
  kXYZ_7 = 7,
};

// Permute the internal STV axes to XYZ order.
template<typename T>
Vec3<T>
toXyz(AxisOrder order, const Vec3<T>& stv)
{
  switch (order) {
  case AxisOrder::kZYX: return {stv.v(), stv.t(), stv.s()};
  case AxisOrder::kXYZ: return {stv.s(), stv.t(), stv.v()};
  case AxisOrder::kXZY: return {stv.s(), stv.v(), stv.t()};
  case AxisOrder::kYZX: return {stv.v(), stv.s(), stv.t()};
  case AxisOrder::kZYX_4: return {stv.v(), stv.t(), stv.s()};
  case AxisOrder::kZXY: return {stv.t(), stv.v(), stv.s()};
  case AxisOrder::kYXZ: return {stv.t(), stv.s(), stv.v()};
  case AxisOrder::kXYZ_7: return {stv.s(), stv.t(), stv.v()};
  }
}

// Permute the an XYZ axis order to the internal STV order.
template<typename T>
Vec3<T>
fromXyz(AxisOrder order, const Vec3<T>& xyz)
{
  switch (order) {
  case AxisOrder::kZYX: return {xyz.z(), xyz.y(), xyz.x()};
  case AxisOrder::kXYZ: return {xyz.x(), xyz.y(), xyz.z()};
  case AxisOrder::kXZY: return {xyz.x(), xyz.z(), xyz.y()};
  case AxisOrder::kYZX: return {xyz.y(), xyz.z(), xyz.x()};
  case AxisOrder::kZYX_4: return {xyz.z(), xyz.y(), xyz.x()};
  case AxisOrder::kZXY: return {xyz.z(), xyz.x(), xyz.y()};
  case AxisOrder::kYXZ: return {xyz.y(), xyz.x(), xyz.z()};
  case AxisOrder::kXYZ_7: return {xyz.x(), xyz.y(), xyz.z()};
  }
}

//============================================================================
// ISO/IEC 23001-8 codec independent code points
enum class ColourMatrix : uint8_t
{
  kIdentity = 0,
  kBt709 = 1,
  kUnspecified = 2,
  kReserved_3 = 3,
  kUsa47Cfr73dot682a20 = 4,
  kBt601 = 5,
  kSmpte170M = 6,
  kSmpte240M = 7,
  kYCgCo = 8,
  kBt2020Ncl = 9,
  kBt2020Cl = 10,
  kSmpte2085 = 11,
};

//============================================================================

// invariant properties
struct AttributeDescription {
  int attr_num_dimensions_minus1;
  int attr_instance_id;

  int bitdepth;
  int bitdepthSecondary;

  int cicp_colour_primaries_idx;
  int cicp_transfer_characteristics_idx;
  ColourMatrix cicp_matrix_coefficients_idx;
  bool cicp_video_full_range_flag;

  AttributeLabel attributeLabel;
};

//============================================================================

struct ProfileCompatibility {
  int profile_compatibility_flags;
};

//============================================================================

struct SequenceParameterSet {
  int sps_seq_parameter_set_id;

  ProfileCompatibility profileCompatibility;
  int level;

  // the bounding box origin (in stv axis order).
  Vec3<int> seqBoundingBoxOrigin;

  // the size of the bounding box (in stv axis order).
  Vec3<int> seqBoundingBoxSize;

  // A value describing the scaling of the source positions prior to encoding.
  float seq_source_geom_scale_factor;

  // NB: attributeSets.size() = num_attribute_sets
  std::vector<AttributeDescription> attributeSets;

  // The number of bits to use for frame_idx
  int log2_max_frame_idx;

  // Defines the ordering of the position components (eg, xyz vs zyx)
  AxisOrder geometry_axis_order;

  // Controls whether bypass bins are written to a seperate sub-stream, or
  // encoded as ep bins via CABAC.
  bool cabac_bypass_stream_enabled_flag;
};

//============================================================================

struct GeometryParameterSet {
  int gps_geom_parameter_set_id;
  int gps_seq_parameter_set_id;

  // Indicates that the GeometryBrickHeader contains a valid
  // geom_box_origin_xyz.
  int geom_box_present_flag;

  // Indicates the presence of gps_geom_box_log2_scale and
  // geom_box_log2_scale.
  bool geom_box_log2_scale_present_flag;

  // Default scaling factor for per-slice geometry box origin
  int gps_geom_box_log2_scale;

  // Controls the ability to represent multiple points (with associated
  // attributes) at the same spatial position.
  bool geom_unique_points_flag;

  // Controls the use of neighbour based contextualisation of octree
  // occupancy during geometry coding.  When true, only neighbours that
  // are direct siblings are available.
  bool neighbour_context_restriction_flag;

  // Defines the size of the neighbour availiability volume (aka
  // look-ahead cube size) for occupancy searches.  A value of 0
  // indicates that the feature is disabled.
  int neighbour_avail_boundary_log2;

  // Controls the use of early termination of the geometry tree
  // by directly coding the position of isolated points.
  bool inferred_direct_coding_mode_enabled_flag;

  // Selects between bitwise and bytewise occupancy coding
  bool bitwise_occupancy_coding_flag;

  // Controlls contextualization of occupancy bits and refinement of
  // the neighbour pattern according to the occupancy of adjacent
  // children in neighbouring nodes.
  bool adjacent_child_contextualization_enabled_flag;

  // Experimental knob to control the number of contexts used
  // for occupancy coding.
  int geom_occupancy_ctx_reduction_factor;

  // Maximum node size where intra prediction is enabled
  int intra_pred_max_node_size_log2;

  // size of triangle nodes (reconstructed surface) in trisoup geometry.
  // a value of zero disables the feature
  int trisoup_node_size_log2;

  // sampling value of trisoup decoding process
  // a value of zero set the automatic sampling value setting to avoid over point of slice MAX points(sliceMaxPoints)
  int trisoup_sampling_value;

  // controls the ability to perform in-loop geometry scaling
  bool geom_scaling_enabled_flag;

  // intial qp for geometry scaling
  int geom_base_qp_minus4;

  // Enables/disables non-cubic geometry nodes
  bool implicit_qtbt_enabled_flag;

  // maximum number of implicit qtbt before performing octree partition
  // for geometry coding.
  int max_num_implicit_qtbt_before_ot;

  // minimum size in log2 of implicit qtbt for geometry coding.
  int min_implicit_qtbt_size_log2;

  // Controls the use of planar mode
  bool geom_planar_mode_enabled_flag;
  int geom_planar_threshold0;
  int geom_planar_threshold1;
  int geom_planar_threshold2;
  int geom_planar_idcm_threshold;

  // Controls the use of xyz-planar mode
  bool geom_angular_mode_enabled_flag;

  // Sequence bounding box relative origin for angular mode computations
  // (in stv axis order).
  Vec3<int> geomAngularOrigin;

  int geom_angular_num_lidar_lasers() const
  {
    return geom_angular_theta_laser.size();
  }

  std::vector<int> geom_angular_theta_laser;
  std::vector<int> geom_angular_z_laser;

  // disable the use of planar buffer when angular mode is enabled
  bool planar_buffer_disabled_flag;

  // implicit qtbt parameters
  int implicit_qtbt_angular_max_node_min_dim_log2_to_split_z;
  int implicit_qtbt_angular_max_diff_to_split_z;
};

//============================================================================

struct GeometryBrickHeader {
  int geom_geom_parameter_set_id;
  int geom_tile_id;
  int geom_slice_id;
  int frame_idx;

  // Origin of the reconstructed geometry, relative to sequence bounding box
  // (in stv axis order).
  Vec3<int> geomBoxOrigin;
  int geom_box_log2_scale;

  // todo(df): minus1?
  int geom_max_node_size_log2;
  Vec3<int> geom_max_node_size_log2_stv;
  int geom_num_points_minus1;

  int geomMaxNodeSizeLog2(const GeometryParameterSet& gps) const
  {
    if (gps.implicit_qtbt_enabled_flag)
      return std::max({geom_max_node_size_log2_stv[0],
                       geom_max_node_size_log2_stv[1],
                       geom_max_node_size_log2_stv[2]});
    return geom_max_node_size_log2;
  }

  Vec3<int> geomMaxNodeSizeLog2Stv(const GeometryParameterSet& gps) const
  {
    if (gps.implicit_qtbt_enabled_flag)
      return geom_max_node_size_log2_stv;
    return geom_max_node_size_log2;
  }

  // qp offset for geometry scaling (if enabled)
  int geom_slice_qp_offset;

  // octree depth at which qp offsets whould be signalled
  int geom_octree_qp_offset_depth;

  // Geometry octree parallel processing is enabled for last N octree depths
  int geom_octree_parallel_max_node_size_log2;
  std::vector<size_t> geom_octree_parallel_bitstream_offsets;
  int geom_octree_parallel_max_offset_log2;

  int geomBoxLog2Scale(const GeometryParameterSet& gps) const
  {
    if (!gps.geom_box_present_flag)
      return 0;
    if (!gps.geom_box_log2_scale_present_flag)
      return gps.gps_geom_box_log2_scale;
    return geom_box_log2_scale;
  }
};

//============================================================================
// NB: when updating this, remember to update AttributeLods::isReusable(...)

struct AttributeParameterSet {
  int aps_attr_parameter_set_id;
  int aps_seq_parameter_set_id;
  AttributeEncoding attr_encoding;

  bool lodParametersPresent() const
  {
    return attr_encoding == AttributeEncoding::kLiftingTransform
      || attr_encoding == AttributeEncoding::kPredictingTransform;
  }

  //--- lifting/predicting transform parameters

  bool lod_decimation_enabled_flag;
  int num_pred_nearest_neighbours_minus1;
  int max_num_direct_predictors;
  int adaptive_prediction_threshold;
  int search_range;

  // NB: in stv order
  Vec3<int32_t> lodNeighBias;

  bool intra_lod_prediction_enabled_flag;
  bool inter_component_prediction_enabled_flag;

  // NB: derived from num_detail_levels_minus1
  int num_detail_levels;
  std::vector<int> lodSamplingPeriod;
  std::vector<int64_t> dist2;

  // NB: these parameters are shared by all transform implementations
  int init_qp_minus4;
  int aps_chroma_qp_offset;
  bool aps_slice_qp_deltas_present_flag;

  //--- raht parameters
  bool raht_prediction_enabled_flag;
  int raht_prediction_threshold0;
  int raht_prediction_threshold1;

  //--- lifting parameters
  bool scalable_lifting_enabled_flag;
};

//============================================================================

struct AttributeBrickHeader {
  int attr_sps_attr_idx;
  int attr_attr_parameter_set_id;
  int attr_geom_slice_id;
  int attr_qp_delta_luma;
  int attr_qp_delta_chroma;

  std::vector<int> attr_layer_qp_delta_luma;
  std::vector<int> attr_layer_qp_delta_chroma;

  bool attr_layer_qp_present_flag() const
  {
    return !attr_layer_qp_delta_luma.empty();
  }

  int attr_num_qp_layers_minus1() const
  {
    return attr_layer_qp_delta_luma.size() - 1;
  }

  // NB: in stv order
  Vec3<int> regionQpOrigin;
  Vec3<int> regionQpSize;
  int attr_region_qp_delta;
  bool attr_region_qp_present_flag;
};

//============================================================================

struct TileInventory {
  struct Entry {
    Vec3<int> tile_bounding_box_xyz0;
    Vec3<int> tile_bounding_box_whd;
  };
  std::vector<Entry> tiles;
};

//============================================================================

void convertXyzToStv(SequenceParameterSet*);
void convertXyzToStv(const SequenceParameterSet&, GeometryParameterSet*);
void convertXyzToStv(const SequenceParameterSet&, AttributeParameterSet*);

//============================================================================

}  // namespace pcc
