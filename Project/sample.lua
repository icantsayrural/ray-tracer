-- Christmas Scene

-- #############################
-- Materials
-- #############################
ground_mat = gr.material({0.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, 0)
rug_mat = gr.material({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 0)
moon_mat = gr.material({1.0, 1.0, 0}, {0, 0, 0}, 0)
leaf_mat = gr.material({0.0, 1.0, 0.0}, {0.0, 0.0, 0.0}, 0)
wood_mat = gr.material({0.2, 0.1, 0}, {0, 0, 0}, 0)
present_mat = gr.material({1.0, 0.0, 0.0}, {0, 0, 0}, 0)
present2_mat = gr.material({0.7, 0.6, 0.1}, {0, 0, 0}, 0)

table_mat = gr.material({0.8, 0.7, 0.7}, {0.7, 0.7, 0.7}, 5.0)

plate_mat = gr.material({0.98, 0.6, 0.08}, {0, 0, 0}, 0)
glass_mat = gr.material({0.0, 0.6, 0.7}, {0.7, 0.7, 0.7}, 5.0)
cookie_mat = gr.material({0.2, 0.0, 0.0}, {0, 0, 0}, 0)

star_mat = gr.material({0.8, 0.8, 0.8}, {1.0, 1.0, 1.0}, 10)

scene = gr.node('scene')
scene:translate(0, -17, 0)

-- #############################
-- Flooring
-- #############################
ground = gr.mesh('ground', 'Assets/plane.obj')
scene:add_child(ground)
ground:set_material(ground_mat)
ground:scale(30, 30, 30)

-- #############################
-- Moon
-- #############################
moon = gr.sphere('moon')
scene:add_child(moon)
moon:set_material(moon_mat)
moon:translate(-8, 30, 0)

-- #############################
-- Tree
-- #############################
trunk = gr.cube('trunk')
ground:add_child(trunk)
trunk:set_material(wood_mat)
trunk:scale(0.05, 0.4, 0.1)
trunk:translate(0.4, 0, -0.4)
trunk:rotate('Y', 15)

leaf = gr.mesh('leaf', 'Assets/leaf.obj')
trunk:add_child(leaf)
leaf:set_material(leaf_mat)
leaf:scale(4, 1, 1)
leaf:translate(0.5, 1.0, 0)

star = gr.mesh('star', 'Assets/dodeca.obj')
leaf:add_child(star)
star:set_material(star_mat)
star:scale(0.2, 0.1, 0.4)
star:translate(0, 1.7, 0)

-- #############################
-- Presents
-- #############################
present = gr.cube('present')
ground:add_child(present)
present:set_material(present_mat)
present:rotate('Y', -17)
present:scale(0.1, 0.1, 0.1)
present:translate(0.1, 0, -0.4)

lid = gr.cube('lid')
present:add_child(lid)
lid:set_material(present_mat)
lid:scale(1.2, 0.3, 1.2)
lid:translate(-0.1, 1, 0)

present2 = gr.cube('present2')
ground:add_child(present2)
present2:set_material(present2_mat)
present2:rotate('Y', 17)
present2:scale(0.1, 0.1, 0.1)
present2:translate(0.3, 0, -0.6)

lid2 = gr.cube('lid2')
present2:add_child(lid2)
lid2:set_material(present2_mat)
lid2:scale(1.2, 0.3, 1.2)
lid2:translate(0, 1, -0.1)

-- #############################
-- Table
-- #############################
table_base = gr.mesh('table_base', 'Assets/cylinder.obj')
ground:add_child(table_base)
table_base:set_material(table_mat)
table_base:scale(0.1, 0.01, 0.1)
table_base:translate(-0.3, 0, -0.4)

table_neck = gr.mesh('table_neck', 'Assets/cylinder.obj')
table_base:add_child(table_neck)
table_neck:set_material(table_mat)
table_neck:scale(0.5, 30, 0.5)

table = gr.mesh('table', 'Assets/cylinder.obj')
table_neck:add_child(table)
table:set_material(table_mat)
table:scale(5, 0.05, 5)
table:translate(0, 1.0, 0)

-- #############################
-- Cookies
-- #############################
plate = gr.mesh('plate', 'Assets/cylinder.obj')
table:add_child(plate)
plate:set_material(plate_mat)
plate:scale(0.5, 0.5, 0.5)
plate:translate(0, 1.45, 0)

cookie = gr.sphere('cookie')
plate:add_child(cookie)
cookie:set_material(cookie_mat)
cookie:scale(0.1, 1.0, 0.2)
cookie:translate(0, 0.5, 0)

cookie2 = gr.sphere('cookie2')
plate:add_child(cookie2)
cookie2:set_material(cookie_mat)
cookie2:scale(0.1, 1.0, 0.2)
cookie2:translate(0.4, 0.5, 0.7)

cookie3 = gr.sphere('cookie3')
plate:add_child(cookie3)
cookie3:set_material(cookie_mat)
cookie3:scale(0.1, 1.0, 0.2)
cookie3:translate(-0.3, 0.5, -0.3)

glass = gr.mesh('glass', 'Assets/cylinder.obj')
table:add_child(glass)
glass:set_material(glass_mat)
glass:scale(0.1, 5, 0.1)
glass:translate(0.7, 1.45, 0)

l1 = gr.light({-100.0, 150, 400}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene, 'sample.png', 1000, 1000,
  {0, 2, 30}, {0, 0, -1}, {0, 1, 0}, 50,
  {0.3, 0.3, 0.3}, {l1})
