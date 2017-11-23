.. py:module:: tilengine

List of classes
===============
This sections contains every class used in the Tilengine python wrapper API, grouped by category with all methods listed.

:class:`Engine`
----------------------------
Main object for engine creation and rendering

* :meth:`Engine.create`
* :meth:`Engine.get_num_objects`
* :meth:`Engine.get_used_memory`
* :meth:`Engine.set_background_color`
* :meth:`Engine.disable_background_color`
* :meth:`Engine.set_background_bitmap`
* :meth:`Engine.set_background_palette`
* :meth:`Engine.set_raster_callback`
* :meth:`Engine.set_render_target`
* :meth:`Engine.update_frame`
* :meth:`Engine.begin_frame`
* :meth:`Engine.draw_next_scanline`
* :meth:`Engine.set_load_path`
* :meth:`Engine.set_custom_blend_function`
* :meth:`Engine.get_available_sprite`
* :meth:`Engine.get_available_animation`

:class:`Window`
----------------------------
Built-in window manager for easy setup and testing

* :meth:`Window.create`
* :meth:`Window.create_threaded`
* :meth:`Window.process`
* :meth:`Window.is_active`
* :meth:`Window.get_input`
* :meth:`Window.enable_input`
* :meth:`Window.assign_joystick`
* :meth:`Window.define_input_key`
* :meth:`Window.define_input_button`
* :meth:`Window.wait_redraw`
* :meth:`Window.enable_crt_effect`
* :meth:`Window.disable_crt_effect`
* :meth:`Window.get_ticks`
* :meth:`Window.delay`
* :meth:`Window.begin_frame`
* :meth:`Window.end_frame`

:class:`Layer`
----------------------------
The Layer object manages each tiled background plane

* :meth:`Layer.setup`
* :meth:`Layer.set_palette`
* :meth:`Layer.set_position`
* :meth:`Layer.set_scaling`
* :meth:`Layer.set_transform`
* :meth:`Layer.set_pixel_mapping`
* :meth:`Layer.reset_mode`
* :meth:`Layer.set_blend_mode`
* :meth:`Layer.set_column_offset`
* :meth:`Layer.set_clip`
* :meth:`Layer.disable_clip`
* :meth:`Layer.set_mosaic`
* :meth:`Layer.disable_mosaic`
* :meth:`Layer.disable`
* :meth:`Layer.get_palette`
* :meth:`Layer.get_tile`

:class:`Sprite`
----------------------------
The Sprite object manages each moving character onscreen

* :meth:`Sprite.setup`
* :meth:`Sprite.set_spriteset`
* :meth:`Sprite.set_flags`
* :meth:`Sprite.set_position`
* :meth:`Sprite.set_picture`
* :meth:`Sprite.set_palette`
* :meth:`Sprite.set_blend_mode`
* :meth:`Sprite.set_scaling`
* :meth:`Sprite.reset_mode`
* :meth:`Sprite.get_picture`
* :meth:`Sprite.enable_collision`
* :meth:`Sprite.check_collision`
* :meth:`Sprite.disable`
* :meth:`Sprite.get_palette`

:class:`Animation`
----------------------------
The Animation object manages each animation for the sequencer engine

* :meth:`Animation.set_palette_animation`
* :meth:`Animation.set_palette_animation_source`
* :meth:`Animation.set_tileset_animation`
* :meth:`Animation.set_sprite_animation`
* :meth:`Animation.get_state`
* :meth:`Animation.set_delay`
* :meth:`Animation.disable`

:class:`Tilemap`
----------------------------
The Tilemap object holds the grid of tiles that define the background layout

* :meth:`Tilemap.create`
* :meth:`Tilemap.fromfile`
* :meth:`Tilemap.clone`
* :meth:`Tilemap.get_tile`
* :meth:`Tilemap.set_tile`
* :meth:`Tilemap.copy_tiles`

:class:`Tileset`
----------------------------
The Tileset object holds the graphic tiles used to render background layers from a Tilemap

* :meth:`Tileset.create`
* :meth:`Tileset.fromfile`
* :meth:`Tileset.clone`
* :meth:`Tileset.set_pixels`
* :meth:`Tileset.copy_tile`

:class:`Spriteset`
----------------------------
The Spriteset object holds the graphic data used to render moving objects (sprites)

* :meth:`Spriteset.create`
* :meth:`Spriteset.fromfile`
* :meth:`Spriteset.clone`
* :meth:`Spriteset.get_info`

:class:`Bitmap`
----------------------------
The Bitmap object holds graphic data used to build in backgrounds, Tileset and Spriteset objects

* :meth:`Bitmap.create`
* :meth:`Bitmap.fromfile`
* :meth:`Bitmap.clone`
* :meth:`Bitmap.get_data`

:class:`Palette`
----------------------------
The Palette object holds the color tables used by tileesets and spritesets to render sprites and backgrounds

* :meth:`Palette.create`
* :meth:`Palette.fromfile`
* :meth:`Palette.clone`
* :meth:`Palette.set_color`
* :meth:`Palette.mix`
* :meth:`Palette.add_color`
* :meth:`Palette.sub_color`
* :meth:`Palette.mod_color`

:class:`SequencePack`
----------------------------
The SequencePack object holds a collection of Sequence objects

* :meth:`SequencePack.create`
* :meth:`SequencePack.fromfile`
* :meth:`SequencePack.find_sequence`
* :meth:`SequencePack.add_sequence`

:class:`Sequence`
----------------------------
The Sequence object holds the sequences to feed the animation engine

* :meth:`Sequence.create_sequence`
* :meth:`Sequence.create_cycle`
* :meth:`Sequence.clone`

Miscellaneous classes
----------------------------
The following classes are used as enumerated types and to pass data to/from some functions

* :class:`WindowFlags`: window creation flags, used by :meth:`Window.create`
* :class:`Flags`: flags for tiles and sprites
* :class:`Blend`: blending modes
* :class:`Input`: inputs list
* :class:`Overlay`: predefined overlay types
* :class:`Tile`: each cell inside a :class:`Tilemap`
* :class:`ColorStrip`: each frame ina color cycle sequence
* :class:`SequenceFrame`: each frame in a :class:`Tilemap`/:class:`Sprite` sequence
* :class:`SpriteInfo`: holds info about a sprite, filled in by :meth:`Spriteset.get_info`
* :class:`TileInfo`: holds info about a tile, filled in by :meth:`Layer.get_tile`
* :class:`SpriteData`: used to feed sprite data for :meth:`Spriteset.create`
* :class:`TileAttributes`: used to feed tile data for :meth:`Tileset.create`
* :class:`PixelMap`: distortion mapping for :meth:`Layer.set_pixel_mapping`
* :class:`Color`: basic RGB color used by all color methods
