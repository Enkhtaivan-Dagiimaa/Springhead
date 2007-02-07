require 'rubygems'
require 'fox16'
require 'fox16/responder'
require 'fox16/undolist'
require 'sprscenetree'
require 'sprpropertymanager'
#require 'sprcameraview'

include Fox

class SprMainWindow < FXMainWindow

	# Load the named icon from a file
	def loadIcon(filename)
		begin
	    	filename = File.join("..", "icons", filename)
	    	icon = nil
	    	File.open(filename, "rb") { |f|
	      		icon = FXPNGIcon.new(getApp(), f.read)
	    	}
		    icon
		  	rescue
	    	raise RuntimeError, "Couldn't load icon: #{filename}"
	  	end
	end

	# constructor
	def initialize(app)
  		# Call base class initialize first
  		super(app, "Springhead GUI", nil, nil, DECOR_ALL, 50, 50, 850, 600, 0, 0)

		# Undoable commands
  		@undolist = FXUndoList.new

    	# Make some icons
    	@bigicon = loadIcon("big.png")
    	@smallicon = loadIcon("small.png")
    	@newicon = loadIcon("filenew.png")
    	@openicon = loadIcon("fileopen.png")
    	@saveicon = loadIcon("filesave.png")
    	@saveasicon = FXPNGIcon.new(getApp(), File.open(File.join("..", "icons", "saveas.png"), "rb").read(), 0, IMAGE_ALPHAGUESS)
    	@printicon = loadIcon("printicon.png")
    	@cuticon = loadIcon("cut.png")
    	@copyicon = loadIcon("copy.png")
    	@pasteicon = loadIcon("paste.png")
    	@deleteicon = loadIcon("kill.png")
    	@undoicon = loadIcon("undo.png")
    	@redoicon = loadIcon("redo.png")
    	@fontsicon = loadIcon("fonts.png")
    	@helpicon = loadIcon("help.png")

		# Application icons
    	setIcon(@bigicon)
    	setMiniIcon(@smallicon)
  
    	# Make menu bar
    	dragshell1 = FXToolBarShell.new(self, FRAME_RAISED|FRAME_THICK)
    	menubar = FXMenuBar.new(self, dragshell1, LAYOUT_SIDE_TOP|LAYOUT_FILL_X)
    	FXToolBarGrip.new(menubar, menubar, FXMenuBar::ID_TOOLBARGRIP, TOOLBARGRIP_DOUBLE)
    
    	# File menu
		filemenu = FXMenuPane.new(self)
    	FXMenuTitle.new(menubar, "&File", nil, filemenu)
  
    	# Edit Menu
		editmenu = FXMenuPane.new(self)
    	FXMenuTitle.new(menubar, "&Edit", nil, editmenu)
  
    	# View menu
		viewmenu = FXMenuPane.new(self)
    	FXMenuTitle.new(menubar, "&View", nil, viewmenu)
  
    	# Options Menu
		optmenu = FXMenuPane.new(self)
    	FXMenuTitle.new(menubar, "&Options", nil, optmenu)
  
    	# Help menu
		helpmenu = FXMenuPane.new(self)
    	FXMenuTitle.new(menubar, "&Help", nil, helpmenu, LAYOUT_RIGHT)
  
		# File Menu entries
    	filenew = FXMenuCommand.new(filemenu, "&New...\tCtl-N\tCreate new document.", @newicon)
		filenew.connect(SEL_COMMAND, method(:onCmdNew))
    	fileopen = FXMenuCommand.new(filemenu, "&Open...        \tCtl-O\tOpen document file.", @openicon)
		fileopen.connect(SEL_COMMAND, method(:onCmdOpen))
    	filereload = FXMenuCommand.new(filemenu, "&Reload...\t\tReload file.", nil)
		filereload.connect(SEL_COMMAND, method(:onCmdReload))
		filereload.connect(SEL_UPDATE, method(:onUpdReload))
    	filesave = FXMenuCommand.new(filemenu, "&Save\tCtl-S\tSave changes to file.", @saveicon)
		filesave.connect(SEL_COMMAND, method(:onCmdSave))
		filesave.connect(SEL_UPDATE, method(:onUpdSave))
    	filesaveas = FXMenuCommand.new(filemenu, "Save &As...\t\tSave document to another file.", @saveasicon)
		filesaveas.connect(SEL_COMMAND, method(:onCmdSaveAs))
    	FXMenuSeparator.new(filemenu)
    	fileinsert = FXMenuCommand.new(filemenu, "Insert from file...\t\tInsert text from file.", nil)
		fileinsert.connect(SEL_COMMAND, method(:onCmdInsert))
		fileinsert.connect(SEL_UPDATE, method(:onUpdInsert))

    	# Recent file menu; this automatically hides if there are no files
    	#sep1 = FXMenuSeparator.new(filemenu)
    	#sep1.setTarget(@mrufiles)
    	#sep1.setSelector(FXRecentFiles::ID_ANYFILES)
    	#FXMenuCommand.new(filemenu, nil, nil, @mrufiles, FXRecentFiles::ID_FILE_1)
    	#FXMenuCommand.new(filemenu, nil, nil, @mrufiles, FXRecentFiles::ID_FILE_2)
    	#FXMenuCommand.new(filemenu, nil, nil, @mrufiles, FXRecentFiles::ID_FILE_3)
    	#FXMenuCommand.new(filemenu, nil, nil, @mrufiles, FXRecentFiles::ID_FILE_4)
    	#FXMenuCommand.new(filemenu, nil, nil, @mrufiles, FXRecentFiles::ID_FILE_5)
    	#FXMenuCommand.new(filemenu, nil, nil, @mrufiles, FXRecentFiles::ID_FILE_6)
    	#FXMenuCommand.new(filemenu, nil, nil, @mrufiles, FXRecentFiles::ID_FILE_7)
    	#FXMenuCommand.new(filemenu, nil, nil, @mrufiles, FXRecentFiles::ID_FILE_8)
    	#FXMenuCommand.new(filemenu, nil, nil, @mrufiles, FXRecentFiles::ID_FILE_9)
    	#FXMenuCommand.new(filemenu, nil, nil, @mrufiles, FXRecentFiles::ID_FILE_10)
    	#FXMenuCommand.new(filemenu, "&Clear Recent Files", nil, @mrufiles, FXRecentFiles::ID_CLEAR)
    	#sep2 = FXMenuSeparator.new(filemenu)
    	#sep2.setTarget(@mrufiles)
    	#sep2.setSelector(FXRecentFiles::ID_ANYFILES)
    	#FXMenuCommand.new(filemenu, "&Quit\tCtl-Q", nil, self, ID_QUIT)
  
    	# Edit Menu entries
    	FXMenuCommand.new(editmenu, "&Undo\tCtl-Z\tUndo last change.", @undoicon)
    	FXMenuCommand.new(editmenu, "&Redo\tCtl-Y\tRedo last undo.", @redoicon)
    	FXMenuSeparator.new(editmenu)
    	FXMenuCommand.new(editmenu, "&Copy\tCtl-C\tCopy selection to clipboard.", @copyicon)
    	FXMenuCommand.new(editmenu, "Cu&t\tCtl-X\tCut selection to clipboard.", @cuticon)
    	FXMenuCommand.new(editmenu, "&Paste\tCtl-V\tPaste from clipboard.", @pasteicon)
    	FXMenuCommand.new(editmenu, "&Delete\t\tDelete selection.", @deleteicon)
    	FXMenuSeparator.new(editmenu)
 
    	# Options menu
    	FXMenuCommand.new(optmenu, "Preferences...\t\tChange preferences.", nil)

    	# Help Menu entries
    	FXMenuCommand.new(helpmenu, "&Help...\t\tDisplay help information.", @helpicon)
    	FXMenuSeparator.new(helpmenu)
    	FXMenuCommand.new(helpmenu, "&About TextEdit...\t\tDisplay about panel.", @smallicon)
  
    	# Tool bar
    	dragshell2 = FXToolBarShell.new(self, FRAME_RAISED|FRAME_THICK)
    	toolbar = FXToolBar.new(self, dragshell2, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT)
    	FXToolBarGrip.new(toolbar, toolbar, FXToolBar::ID_TOOLBARGRIP, TOOLBARGRIP_DOUBLE)
  
		# Toobar buttons: File manipulation
		FXButton.new(toolbar, "New\tNew\tCreate new document.", @newicon, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT))
		FXButton.new(toolbar, "Open\tOpen\tOpen document file.", @openicon, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT))
		FXButton.new(toolbar, "Save\tSave\tSave document.", @saveicon, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT))
		FXButton.new(toolbar, "Save as\tSave As\tSave document to another file.", @saveasicon, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT))

		# Toobar buttons: Editing
		FXFrame.new(toolbar, LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT, 0, 0, 5, 5)
		FXButton.new(toolbar, "Cut\tCut\tCut selection to clipboard.", @cuticon, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT))
		FXButton.new(toolbar, "Copy\tCopy\tCopy selection to clipboard.", @copyicon, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT))
		FXButton.new(toolbar, "Paste\tPaste\tPaste clipboard.", @pasteicon, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT))
		FXButton.new(toolbar, "Undo\tUndo\tUndo last change.", @undoicon, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT))
		FXButton.new(toolbar, "Redo\tRedo\tRedo last undo.", @redoicon, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT))

		FXButton.new(toolbar, "Help\tHelp on editor\tDisplay help information.", @helpicon, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_RIGHT))

    	# Time control tool bar
    	dragshell3 = FXToolBarShell.new(self, FRAME_RAISED|FRAME_THICK)
    	toolbar = FXToolBar.new(self, dragshell3, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT)
    	FXToolBarGrip.new(toolbar, toolbar, FXToolBar::ID_TOOLBARGRIP, TOOLBARGRIP_DOUBLE)
		FXButton.new(toolbar, "Start\tStart\tStart simulation.", 	nil, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT)).connect(SEL_COMMAND, method(:onSimStart))
		FXButton.new(toolbar, "Stop\tStop\tStop simulation.", 		nil, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT)).connect(SEL_COMMAND, method(:onSimStop))
		FXButton.new(toolbar, "Forward\tForward\tStep forward.", 	nil, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT)).connect(SEL_COMMAND, method(:onSimForward))
		FXButton.new(toolbar, "Backward\tBackward\tStep backward.", nil, nil, 0, (ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT))
		

    	# Status bar
    	statusbar = FXStatusBar.new(self, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|STATUSBAR_WITH_DRAGCORNER)

    	# Make a tool tip
    	FXToolTip.new(getApp(), 0)
  
    	# Recent files
    	@mrufiles = FXRecentFiles.new  

    	# �V�[���O���t�c���[���v���p�e�B�i���j�ƃf�B�X�v���C�i�E�j��Splitter
    	@hsplitter = FXSplitter.new(self, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_TRACKING)
  
    	# �V�[���O���t�c���[�i��j�ƃv���p�e�B�i���j��Splitter
    	@vsplitter = FXSplitter.new(@hsplitter, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y|SPLITTER_VERTICAL|SPLITTER_TRACKING)
  
		# �V�[���O���t�c���[
    	treeframe = FXHorizontalFrame.new(@vsplitter, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0)
	    $scenetree = SprSceneTree.new(treeframe)

		# �v���p�e�B
		propertyframe = FXHorizontalFrame.new(@vsplitter)
	    $propertymanager = SprPropertyManager.new(propertyframe)

	    # �f�B�X�v���C
    	cameraframe = FXHorizontalFrame.new(@hsplitter, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0)
	    @glvisual = FXGLVisual.new(getApp(), VISUAL_DOUBLEBUFFER)
	    @glcanvas = FXGLCanvas.new(cameraframe, @glvisual, nil, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT)
	    @glcanvas.connect(SEL_PAINT){drawScene}
	    @glcanvas.connect(SEL_CONFIGURE) {
	    	if @glcanvas.makeCurrent
	        	$sprapp.GetSdk().GetRender().SetViewport([0, 0], [@glcanvas.width, @glcanvas.height])
	        	@glcanvas.makeNonCurrent
	      	end
	    }
    
    	# Add some alternative accelerators
    	if getAccelTable()
    	  	getAccelTable().addAccel(MKUINT(KEY_Z, CONTROLMASK|SHIFTMASK), @undolist, MKUINT(FXUndoList::ID_REDO, SEL_COMMAND))
    	end
  
    	# Initialize file name
    	@filename = "untitled"
    	@filetime = nil
    	@filenameset = false
  
    	# Initialize other stuff
    	@undolist.mark
	end

	# draw scene
	def	drawScene()

	    # Make context current
    	@glcanvas.makeCurrent()

		$sprapp.GetSdk().GetRender().Reshape([0,0], [@glcanvas.width, @glcanvas.height])
		$sprapp.Display()
		
		# Swap if it is double-buffered
    	if @glvisual.isDoubleBuffer
      		@glcanvas.swapBuffers
    	end
    
    	# Make context non-current
    	@glcanvas.makeNonCurrent
	end

  	# Load file
  	def loadFile(filename)
    	begin
      		getApp().beginWaitCursor()
      	
			$sprapp.GetSdk().LoadScene(filename)

    	ensure
      		getApp().endWaitCursor()
    	end

    	# Set stuff
    	@mrufiles.appendFile(filename)
    	@filetime = File.mtime(filename)
    	@filename = filename
    	@filenameset = true
    	@undolist.clear
    	@undolist.mark

		$scenetree.update
  	end

	# Insert file
	def insertFile(file)
	  	begin
	    	getApp().beginWaitCursor()
	    	#text = File.open(file, "r").read
	  	ensure
	  	  	getApp().endWaitCursor()
	  	end
	end

  	# Save file
  	def saveFile(file)
    	# Set wait cursor
    	getApp().beginWaitCursor()

    	# Write the file
    	#File.open(file, "w").write(text)

    	# Kill wait cursor
    	getApp().endWaitCursor()

    	# Set stuff
    	@mrufiles.appendFile(file)
    	@filetime = File.mtime(file)
    	@filename = file
    	@filenameset = true
    	@undolist.mark
  	end

  	# Read settings from registry
  	def readRegistry
	
	end

	# Save settings to registry
  	def writeRegistry
    
  	end

  	# Save changes, prompt for new filename
  	def saveChanges
    	if !@undolist.marked?
      		answer = FXMessageBox.question(self, MBOX_YES_NO_CANCEL,
        	"Unsaved Document", "Save current document to file?")
      		return false if (answer == MBOX_CLICKED_CANCEL)
      		if answer == MBOX_CLICKED_YES
        		file = @filename
        		if !@filenameset
          			savedialog = FXFileDialog.new(self, "Save Document")
          			savedialog.selectMode = SELECTFILE_ANY
          			savedialog.patternList = getPatterns()
          			savedialog.currentPattern = getCurrentPattern()
          			savedialog.filename = file
          			return false if (savedialog.execute == 0)
          			setCurrentPattern(savedialog.currentPattern)
          			file = savedialog.filename
          			if File.exists?(file)
            			if MBOX_CLICKED_NO == FXMessageBox.question(self, MBOX_YES_NO,
            				  "Overwrite Document", "Overwrite existing document: #{file}?")
              				return false
            			end
          			end
        		end
        		file = savedialog.filename
        		saveFile(file)
      		end
    	end
    	true
  	end

	#--------------------------------------------------------------------------------
	# message handlers

  	# New
  	def onCmdNew(sender, sel, ptr)
    	return 1 if !saveChanges()

		$sprapp.Clear()

    	@filename = "untitled"
    	@filetime = nil
    	@filenameset = false
    	@editor.text = nil
    	@editor.modified = false
    	@editor.editable = true
    	@undolist.clear
    	@undolist.mark
    	return 1
  	end


  	# Open
  	def onCmdOpen(sender, sel, ptr)
    	return 1 if !saveChanges()

    	opendialog = FXFileDialog.new(self, "Open Document")
    	opendialog.selectMode = SELECTFILE_EXISTING
    	#opendialog.patternList = getPatterns()
    	#opendialog.currentPattern = getCurrentPattern()
    	opendialog.filename = @filename
    	if opendialog.execute != 0
      		#setCurrentPattern(opendialog.currentPattern)
      		loadFile(opendialog.filename)
    	end
    	return 1
  	end

  	# Reload file
  	def onCmdReload(sender, sel, ptr)
    	if !@undolist.marked?
      		if FXMessageBox.question(self, MBOX_YES_NO, "Document was changed",
        		"Discard changes to this document?") == MBOX_CLICKED_NO
           		return 1
      		end
    	end
    	loadFile(@filename)
    	return 1
  	end

  	# Update reload file
  	def onUpdReload(sender, sel, ptr)
    	if @filenameset
      		sender.handle(self, MKUINT(ID_ENABLE, SEL_COMMAND), nil)
    	else
      		sender.handle(self, MKUINT(ID_DISABLE, SEL_COMMAND), nil)
    	end
    	return 1
  	end

  	# Insert file into buffer
  	def onCmdInsert(sender, sel, ptr)
    	opendialog = FXFileDialog.new(self, "Open Document")
    	opendialog.selectMode = SELECTFILE_EXISTING
    	opendialog.patternList = getPatterns()
    	opendialog.currentPattern = getCurrentPattern()
    	if opendialog.execute != 0
      		setCurrentPattern(opendialog.currentPattern)
      		insertFile(opendialog.filename)
    	end
    	return 1
  	end

  	# Update insert file
  	def onUpdInsert(sender, sel, ptr)
    	#if @editor.editable?
      	#	sender.handle(self, MKUINT(ID_ENABLE, SEL_COMMAND), nil)
    	#else
      	#	sender.handle(self, MKUINT(ID_DISABLE, SEL_COMMAND), nil)
    	#end
    	return 1
  	end

  	# Save
  	def onCmdSave(sender, sel, ptr)
    	if !@filenameset
      		return onCmdSaveAs(sender, sel, ptr)
    	end
    	saveFile(@filename)
    	return 1
  	end

  	# Save Update
  	def onUpdSave(sender, sel, ptr)
    	msg = (!@undolist.marked?) ? FXWindow::ID_ENABLE : FXWindow::ID_DISABLE
    	sender.handle(self, MKUINT(msg, SEL_COMMAND), nil)
    	return 1
  	end

  	# Save As
  	def onCmdSaveAs(sender, sel, ptr)
    	savedialog = FXFileDialog.new(self, "Save Document")
    	file = @filename
    	savedialog.selectMode = SELECTFILE_ANY
    	savedialog.patternList = getPatterns()
    	savedialog.currentPattern = getCurrentPattern()
    	savedialog.filename = file
    	if savedialog.execute != 0
      		setCurrentPattern(savedialog.currentPattern)
      		file = savedialog.filename
      		if File.exists?(file)
        		if MBOX_CLICKED_NO == FXMessageBox.question(self, MBOX_YES_NO,
          			"Overwrite Document", "Overwrite existing document: #{file}?")
          			return 1
        		end
      		end
      		saveFile(file)
    	end
    	return 1
  	end

  	# Save settings
  	def onCmdSaveSettings(sender, sel, ptr)
    	writeRegistry();
    	getApp().reg().write
    	return 1
  	end

  	# Quit
  	def onCmdQuit(sender, sel, ptr)
    	return 1 if !saveChanges()
    	writeRegistry()
    	getApp().exit(0)
    	return 1
  	end

  	# Update title
  	def onUpdTitle(sender, sel, ptr)
    	title = "FOX Text Editor:- " + @filename
    	title += "*" if !@undolist.marked?
    	sender.handle(self, MKUINT(FXWindow::ID_SETSTRINGVALUE, SEL_COMMAND), title)
    	return 1
  	end

	# start simulation
	def	onSimStart(sender, sel, ptr)
		@simRunning = true
		@timer = getApp().addTimeout(100, method(:onTimeout))
	end

	# stop simulation
	def onSimStop(sender, sel, ptr)
		@simRunning = false
	end

	# step forward
	def onSimForward(sender, sel, ptr)
		$sprapp.Step()
		drawScene()
	end

	# time handler
	def onTimeout(sender, sel, ptr)
		$sprapp.Step()
		if @simRunning
			@timer = getApp().addTimeout(100, method(:onTimeout))
		end
		drawScene()
	end

 	# Released right button
  	def onTextRightMouse(sender, sel, event)
    	if !event.moved
      	pane = FXMenuPane.new(self)
      	FXMenuCommand.new(pane, "Undo", @undoicon, @undolist, FXUndoList::ID_UNDO)
      	FXMenuCommand.new(pane, "Redo", @redoicon, @undolist, FXUndoList::ID_REDO)
      	FXMenuSeparator.new(pane)
      	FXMenuCommand.new(pane, "Cut", @cuticon, @editor, FXText::ID_CUT_SEL)
      	FXMenuCommand.new(pane, "Copy", @copyicon, @editor, FXText::ID_COPY_SEL)
      	FXMenuCommand.new(pane, "Paste", @pasteicon, @editor, FXText::ID_PASTE_SEL)
      	FXMenuCommand.new(pane, "Select All", nil, @editor, FXText::ID_SELECT_ALL)
      	pane.create
      	pane.popup(nil, event.root_x, event.root_y)
      	getApp().runModalWhileShown(pane)
   		end
    	return 1
  	end

 	# Show preferences dialog
  	def onCmdPreferences(sender, sel, ptr)
    	preferences = PrefDialog.new(self)
    	preferences.setPatterns(getPatterns())
    	if preferences.execute != 0
      		setPatterns(preferences.getPatterns())
    	end
    	return 1
  	end

  	# Show help window
  	def onCmdShowHelp(sender, sel, ptr)
    	@helpwindow.show(PLACEMENT_CURSOR)
    	return 1
  	end

  	# About box
  	def onCmdAbout(sender, sel, ptr)
    	about = FXMessageBox.new(self, "Springhead GUI",
      	"Springhead GUI", @bigicon, MBOX_OK|DECOR_TITLE|DECOR_BORDER)
    	about.execute
    	return 1
  	end


	#--------------------------------------------------------------------------------
	# message handlers

	# Start the ball rolling
	def start(args)
    	if args.length > 0
      		loadFile(File.expand_path(args[1]))
    	end
  	end

  	# Create and show window
  	def create
    	@urilistType = getApp().registerDragType(FXWindow.urilistTypeName) unless @urilistType
    	readRegistry
    	super
		# ���y�C���̕�
		@hsplitter.setSplit(0, $scenetree.font.getTextWidth('MMMMMMMMMMMMMMMM'))
		# ���y�C���̏㉺������
		@vsplitter.setSplit(0, @vsplitter.getHeight() * 0.7);

  		show(PLACEMENT_SCREEN)
  	end

end
