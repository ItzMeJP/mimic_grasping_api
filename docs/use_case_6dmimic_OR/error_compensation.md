# 6DMimic + OR error estimation

# TODO

It is possible to correct estimation error (compensate it) by setting one of the above files:

- **tool_error_compensation.json:** set of compensation heuristics to correct the measurements that come from tool localization, i.e. the tool pose w.r.t. tool localization base frame.
- **obj_error_compensation.json:** set of compensation heuristics to correct the measurements that come from object localization, i.e. the object pose w.r.t. tool localization base frame.
- **general_error_compensation.json:** set of compensation heuristics to correct the measurements that come from overall system, i.e. the grasping pose candidate w.r.t. object frame.

Four types of compensations are allowed:

- **Constant Absolute:**
- **Constant Relative:**
- **Linear:**
- **Exponential:**


To the discussed use case, the errors were acquired by using the ground_truth piece, image below. This piece is defined by 1.0mm-width cuts. Placing the point tool in these cuts can define the expected pose. Thus, for each axis 6 different poses are defined along these points. The mimic tool is set with the probe point, i.e. none gripper is used. Be notice that the tip.pnt file in 6DMimic pc should be defined according to this new tool configuration!
