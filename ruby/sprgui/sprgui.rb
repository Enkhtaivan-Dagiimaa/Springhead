#!/usr/bin/env ruby

require '../Springhead'
require '../PTM'
require 'rubygems'
require 'fox16'
require 'fox16/responder'
require 'fox16/undolist'
require 'sprmainwindow'

include Fox
include Springhead
include PTM

$sprapp

class SprGUIApp < FXApp
	def initialize()
		super("Springhead GUI", "Springhead")

		# GL������
	    $glvisual = FXGLVisual.new(self, VISUAL_NOACCEL | VISUAL_TRUECOLOR | VISUAL_DOUBLEBUFFER | VISUAL_SWAP_COPY)
		$glvisual.redSize = 5
		$glvisual.greenSize = 5
		$glvisual.blueSize = 5
		$glvisual.depthSize = 16
		
		# Springhead������
		$sprapp = FWAppGL.new
		$sprapp.Link
		$sprapp.Init(1 + ARGV.size, [$0] + ARGV)
		$sprapp.GetSdk.SetDebugMode()

		# Make window
		@window = SprMainWindow.new(self)
		$cameraview.makeCurrent
		$sprapp.CreateRender
		$cameraview.makeNonCurrent

		threadsEnabled = false

		# Open display
		init(ARGV)
			
		# Handle interrupt to save stuff nicely
		# application.addSignal("SIGINT", window, TextWindow::ID_QUIT)
		
		# Create it
		create
		
		# Start
		#@window.start(ARGV)
		
		# Run
		run
	end
end

# Start the whole thing
if __FILE__ == $0
	# Make application
	application = SprGUIApp.new
	
end
