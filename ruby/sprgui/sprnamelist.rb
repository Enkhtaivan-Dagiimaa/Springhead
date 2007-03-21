include Fox

class SprNameList < FXComboBox
	SOLIDS = 0
	JOINTS = 1
	SHAPES = 2
	JOINTTYPE = 3
	def initialize(owner, type=nil)
		super(owner, 25, nil, 0, COMBOBOX_STATIC|LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK)
		numVisible = 4

		update(type) if type
	end

	def update(type)
		clearItems
		phsdk = $sprapp.GetSdk.GetPHSdk
		phscene = $sprapp.GetSdk.GetScene.GetPHScene
		if type == SOLIDS
			for i in 0..phscene.NSolids()-1
				solid = phscene.GetSolids()[i]
				appendItem(solid.GetName(), solid)
			end
		elsif type == JOINTS
			for i in 0..phscene.NJoints()-1
				joint = phscene.GetJoints(i)
				appendItem(joint.GetName(), joint)
			end
		elsif type == SHAPES
			for i in 0..phsdk.NShapes()-1
				shape = phsdk.GetShape(i)
				appendItem(shape.GetName(), shape)
			end
		elsif type == JOINTTYPE
			appendItem('hinge')
			appendItem('slider')
			appendItem('balljoint')
			appendItem('spring')
		end
	end
end