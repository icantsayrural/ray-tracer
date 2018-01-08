-- Materials
mt_wall = gr.material({0.84, 0.6, 0.53}, {0, 0, 0}, 0, 0, 0, 0, "", "brick.png")
mt_countertop = gr.material({0, 0, 0}, {0.2, 0.2, 0.2}, 20, 0.001, 0, 0, "marble.png", "")

mt_kettle = gr.material({0, 0, 0}, {0.2, 0.2, 0.2}, 2, 0.2, 0, 0, "", "")
mt_plate = gr.material({1, 1, 1}, {0.2, 0.2, 0.2}, 3, 0, 0, 0, "", "")
mt_cheese = gr.material({1, 1, 0}, {0, 0, 0}, 0, 0.001, 0, 0, "", "")
mt_bottle = gr.material({0, 0.6, 0}, {0, 0, 0}, 0, 0.001, 0.4, 1.62, "", "")
mt_glass = gr.material({0, 0, 0}, {0.5, 0.5, 0.5}, 1, 0, 0.5, 1.62, "", "")

mt_handle = gr.material({0, 0, 1}, {0, 0, 0}, 0, 0, 0, 0, "", "")
mt_blade = gr.material({0, 0, 0}, {0.2, 0.3, 0.2}, 20, 0.001, 0, 0, "", "")

mt_bubble = gr.material({0, 0, 1}, {0.2, 0.2, 0.2}, 2, 0, 0, 0, "", "")

-- Width of the table
width = 60

scene = gr.node('scene')
scene:rotate('X', 25)
scene:translate(-width/2, 0, -25)

-- Walls
background = gr.cube('background')
scene:add_child(background)
background:set_material(mt_wall)
background:scale(width, 20, 1)
background:translate(0, 0, 0)

left_wall = gr.cube('left_wall')
scene:add_child(left_wall)
left_wall:set_material(mt_wall)
left_wall:scale(1, 20, 21)
left_wall:translate(-1, 0, 0)

right_wall = gr.cube('right_wall')
scene:add_child(right_wall)
right_wall:set_material(mt_wall)
right_wall:scale(1, 20, 21)
right_wall:translate(width, 0, 0)

-- Countertop
countertop = gr.cube('countertop')
scene:add_child(countertop)
countertop:set_material(mt_countertop)
countertop:scale(width, 1, 20)
countertop:translate(0, 0, 1)

-- objects
kettle = gr.mesh('kettle', 'teapot.obj')
countertop:add_child(kettle)
kettle:set_material(mt_kettle)
kettle:rotate('Y', 180)
kettle:scale(0.04, 2.0, 0.15)
kettle:translate(0.65, -1.3, 0.2)

ps = gr.particles('ps')
kettle:add_child(ps)
ps:set_material(mt_bubble)
ps:scale(0.1, 0.1, 0.1)
ps:translate(2.5, 4, 0.8)

plate = gr.mesh('plate', 'plate.obj')
countertop:add_child(plate)
plate:set_material(mt_plate)
plate:scale(0.05, 2.9, 0.05)
plate:translate(0.8, 0.5, 0.75)

cheese = gr.cube('cheese')
plate:add_child(cheese)
cheese:set_material(mt_cheese)
cheese:translate(-5, 0.8, 0.8)

hole_number = 1
for _, pt in pairs({
  {{0.2, 0.25, 0.25}, {0, 1, 0.3}},
  {{0.1, 0.1, 0.1}, {0, 0.5, 0.6}},
  {{0.15, 0.15, 0.15}, {0.5, 0.7, 0.6}},
  {{0.1, 0.1, 0.1}, {0.3, 0.5, 0.6}}
}) do
  cheese_hole = gr.sphere('cheese_hole' .. tostring(hole_number))
  cheese:add_child(cheese_hole)
  cheese_hole:set_material(mt_cheese)
  cheese_hole:scale(table.unpack(pt[1]))
  cheese_hole:translate(table.unpack(pt[2]))

  hole_number = hole_number + 1
end

bottle = gr.mesh('bottle', 'bottle.obj')
countertop:add_child(bottle)
bottle:set_material(mt_bottle)
bottle:scale(0.015, 0.75, 0.085)
bottle:translate(0.44, 1.5, 0.8)

glass = gr.mesh('glass', 'glass.obj')
countertop:add_child(glass)
glass:set_material(mt_glass)
glass:scale(0.009, 0.5, 0.009)
glass:translate(0.4, 4, 0.53)

knife_handle = gr.mesh('knife_handle', 'knife_handle.obj')
countertop:add_child(knife_handle)
knife_handle:set_material(mt_handle)
knife_handle:rotate('Y', 180)
knife_handle:scale(0.01, 1, 0.05)
knife_handle:translate(0.46, 1.5, 0.7)

knife_blade = gr.mesh('knife_blade', 'knife_blade.obj')
knife_handle:add_child(knife_blade)
knife_blade:set_material(mt_blade)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'project', 500, 500,
{0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})
