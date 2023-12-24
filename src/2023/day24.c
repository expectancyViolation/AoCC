#include "day24.h"
#include "../../res/hashmap.c/hashmap.h"
#include <gmp.h>
#include <stdint-gcc.h>

typedef struct {
  mpz_t x;
  mpz_t y;
  mpz_t z;
} MyVec3D;

typedef struct {
  MyVec3D start_position;
  mpz_t position_sum;
  MyVec3D velocity;
  mpz_t velocity_sum;
} D24Hailstone;

static MyVec3D parse_my_vec3D(char *line) {
  long x = strtol(line, &line, 10);
  line++;
  long y = strtol(line, &line, 10);
  line++;
  long z = strtol(line, &line, 10);
  line++;
  MyVec3D res = {0};
  mpz_init_set_si(res.x, x);
  mpz_init_set_si(res.y, y);
  mpz_init_set_si(res.z, z);
  return res;
}

static void my_vec3D_print(MyVec3D const *vec) {
  mpz_out_str(stdout, 10, vec->x);
  printf(", ");
  mpz_out_str(stdout, 10, vec->y);
  printf(", ");
  mpz_out_str(stdout, 10, vec->z);
}

MyVec3D my_vec3d_cross(MyVec3D const *v1, MyVec3D const *v2) {
  MyVec3D res = {0};
  mpz_init(res.x);
  mpz_init(res.y);
  mpz_init(res.z);
  mpz_t buff1;
  mpz_t buff2;
  mpz_init_set_si(buff1, 0);
  mpz_init_set_si(buff2, 0);
  mpz_mul(buff1, v1->y, v2->z);
  mpz_mul(buff2, v1->z, v2->y);
  mpz_sub(res.x, buff1, buff2);

  mpz_mul(buff1, v2->x, v1->z);
  mpz_mul(buff2, v2->z, v1->x);
  mpz_sub(res.y, buff1, buff2);

  mpz_mul(buff1, v1->x, v2->y);
  mpz_mul(buff2, v1->y, v2->x);
  mpz_sub(res.z, buff1, buff2);
  return res;
}

void my_vec3d_sum(MyVec3D const v1, mpz_t out) {
  mpz_set_si(out, 0);
  mpz_add(out, v1.x, v1.y);
  mpz_add(out, out, v1.z);
}

void my_vec3d_dot(MyVec3D const v1, MyVec3D const v2, mpz_t out) {
  mpz_set_si(out, 0);
  mpz_addmul(out, v1.x, v2.x);
  mpz_addmul(out, v1.y, v2.y);
  mpz_addmul(out, v1.z, v2.z);
}

static D24Hailstone parse_hailstone(char *line) {
  MyVec3D const position = parse_my_vec3D(strsep(&line, "@"));

  MyVec3D const velocity = parse_my_vec3D(line);
  D24Hailstone res = {.start_position = position, .velocity = velocity};
  mpz_init_set_si(res.position_sum, 0);
  my_vec3d_sum(position, res.position_sum);

  mpz_init_set_si(res.velocity_sum, 0);
  my_vec3d_sum(velocity, res.velocity_sum);

  return res;
}

static MyVec3D get_hom(mpz_t const x, mpz_t const y) {
  MyVec3D res = {0};
  mpz_init_set(res.x, x);
  mpz_init_set(res.y, y);
  mpz_init_set_si(res.z, 1);
  return res;
}

static MyVec3D get_xy_line_proj(D24Hailstone const *hs) {
  MyVec3D p0_proj = get_hom(hs->start_position.x, hs->start_position.y);
  mpz_t p1x;
  mpz_init(p1x);
  mpz_add(p1x, hs->start_position.x, hs->velocity.x);

  mpz_t p1y;
  mpz_init(p1y);
  mpz_add(p1y, hs->start_position.y, hs->velocity.y);
  MyVec3D p1_proj = get_hom(p1x, p1y);

  return my_vec3d_cross(&p0_proj, &p1_proj);
}

static bool point_is_time_forward_on_hailstone_path(D24Hailstone const *hs,
                                                    MyVec3D point) {
  mpz_t dot1;
  mpz_init_set_si(dot1, 0);
  my_vec3d_dot(get_hom(hs->velocity.x, hs->velocity.y),
               get_hom(hs->start_position.x, hs->start_position.y), dot1);

  mpz_mul(dot1, dot1, point.z);

  mpz_t dot2;
  mpz_init_set_si(dot2, 0);
  my_vec3d_dot(get_hom(hs->velocity.x, hs->velocity.y), point, dot2);
  mpz_t res;
  mpz_init_set_si(res, 0);
  mpz_sub(res, dot2, dot1);
  mpz_mul(res, res, point.z); // use z again just for the sign
  return mpz_sgn(res) >= 0;
}

static bool
hailstone_path_xy_intersect_in_future(D24Hailstone const *hs1,
                                      D24Hailstone const *hs2,
                                      MyVec3D *out_intersection_proj) {
  MyVec3D const hs1_xy_line_proj = get_xy_line_proj(hs1);
  MyVec3D const hs2_xy_line_proj = get_xy_line_proj(hs2);
  *out_intersection_proj = my_vec3d_cross(&hs1_xy_line_proj, &hs2_xy_line_proj);
  bool intersection_is_finite = mpz_sgn(out_intersection_proj->z) != 0;
  return intersection_is_finite &&
         point_is_time_forward_on_hailstone_path(hs1, *out_intersection_proj) &&
         point_is_time_forward_on_hailstone_path(hs2, *out_intersection_proj);
}

bool crt(mpz_t mod1, mpz_t rem1, mpz_t const mod2, mpz_t const rem2,
         mpz_t out_sol, mpz_t out_lcm) {

  mpz_mod(rem1, rem1, mod1);
  mpz_mod(rem2, rem2, mod2);
  bool possible = true;
  mpz_t s;
  mpz_t t;
  mpz_t gcd_;
  mpz_init_set_si(s, 0);
  mpz_init_set_si(t, 0);
  mpz_init_set_si(gcd_, 0);
  mpz_gcdext(gcd_, s, t, mod1, mod2);

  mpz_t diff;
  if (mpz_cmp_si(gcd_, 1) > 0) {
    mpz_init_set_si(diff, 0);
    mpz_sub(diff, rem1, rem2);
    mpz_mod(diff, diff, gcd_);
    possible = mpz_cmp_si(diff, 0) == 0;
  }

  if (possible) {
    mpz_mul(s, s, mod1);
    mpz_mul(s, s, rem2);
    mpz_mul(t, t, mod2);
    mpz_mul(t, t, rem1);
    mpz_add(out_sol, s, t);
    mpz_fdiv_q(out_sol, out_sol, gcd_);
    mpz_fdiv_q(out_lcm, mod1, gcd_);
    mpz_mul(out_lcm, out_lcm, mod2);
    mpz_mod(out_sol, out_sol, out_lcm);
  }
  return possible;
}

// solve by comparing sums of variables:
//  sum of difference of velocities must divide into sum of position difference
//  => get modular equations solvable by CRT
//  => actual start position solution must be "reasonably" sized to allow for
//  enough collision times
long long solve_part2(D24Hailstone const *hailstones_vec) {
  mpz_t velocity_candidate;
  mpz_init_set_si(velocity_candidate, 0);
  for (int i = -1000; i < 1000; i++) {
    mpz_set_si(velocity_candidate, i);
    mpz_t velocity_diff;
    mpz_init_set_si(velocity_diff, 0);
    mpz_t overall_rem;
    mpz_init_set_si(overall_rem, 0);
    mpz_t overall_mod;
    mpz_init_set_si(overall_mod, 1);
    bool possible = false;
    for (int j = 0; j < 20; j++) {
      mpz_t curr_rem;
      mpz_init_set(curr_rem, overall_rem);
      mpz_t curr_mod;
      mpz_init_set(curr_mod, overall_mod);
      mpz_sub(velocity_diff, velocity_candidate,
              hailstones_vec[j].velocity_sum);
      mpz_abs(velocity_diff, velocity_diff);
      if (mpz_cmp_si(velocity_diff, 0) == 0) {
        possible = false;
        break;
      }
      mpz_t rem2;
      mpz_init_set(rem2, hailstones_vec[j].position_sum);
      possible = crt(curr_mod, curr_rem, velocity_diff, rem2, overall_rem,
                     overall_mod);
      if (!possible)
        break;
    }
    if (possible) {
      mpz_t lim;
      mpz_init_set_si(lim, 10);
      mpz_pow_ui(lim, lim, 20);
      if (mpz_cmp(overall_rem, lim) <= 0) {
        return mpz_get_si(overall_rem);
      }
    }
  }
}

LLTuple year23_day24(char *buf, long buf_len) {
  LLTuple res = {0};
  D24Hailstone *hailstones_vec = NULL;
  while (buf != NULL) {
    char *line = strsep(&buf, "\n");
    cvector_push_back(hailstones_vec, parse_hailstone(line));
  }
  const signed long lb = 200000000000000;
  const signed long ub = 400000000000000;
  mpz_t lower_bound;
  mpz_init_set_si(lower_bound, lb);
  mpz_t upper_bound;
  mpz_init_set_si(upper_bound, ub);
  int intersection_count = 0;
  for (int i = 0; i < cvector_size(hailstones_vec); i++) {
    D24Hailstone const *hs1 = &(hailstones_vec[i]);
    for (int j = i + 1; j < cvector_size(hailstones_vec); j++) {
      D24Hailstone const *hs2 = &(hailstones_vec[j]);
      MyVec3D intersection_proj = {0};
      if (hailstone_path_xy_intersect_in_future(hs1, hs2, &intersection_proj)) {
        mpz_t scale;
        mpz_init_set(scale, intersection_proj.z);
        mpz_t lower_bound_scaled;
        mpz_init(lower_bound_scaled);
        mpz_mul(lower_bound_scaled, lower_bound, scale);

        mpz_t upper_bound_scaled;
        mpz_init(upper_bound_scaled);
        mpz_mul(upper_bound_scaled, upper_bound, scale);

        int zsign = mpz_sgn(intersection_proj.z);
        int invert_signs = (zsign < 0) ? (-1) : 1;

        bool intersection_inside_bounds =
            ((mpz_cmp(lower_bound_scaled, intersection_proj.x) *
              invert_signs) <= 0) &&
            ((mpz_cmp(intersection_proj.x, upper_bound_scaled) *
              invert_signs) <= 0) &&
            ((mpz_cmp(lower_bound_scaled, intersection_proj.y) *
              invert_signs) <= 0) &&
            ((mpz_cmp(intersection_proj.y, upper_bound_scaled) *
              invert_signs) <= 0);
        intersection_count += intersection_inside_bounds;
      }
    }
  }
  res.left = intersection_count;
  res.right = solve_part2(hailstones_vec);
  return res;
}
AocDayRes solve_year23_day24(const char *input_file) {
  char *input_buffer;
  const long filesize = read_file_to_memory(input_file, &input_buffer, true);
  const LLTuple res = year23_day24(input_buffer, filesize);
  AocDayRes day_res = aoc_day_res_from_tuple(&res);
  free(input_buffer);
  return day_res;
}
