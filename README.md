CmdAndConquer Change Log:

TO DO LIST:
	N/A

VERSION LIST:
0.0001 - Included features: 

					#Opening files
					#vertical and horizontal scrolling
					
0.0002 - Included features: 
					
					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					
0.0003 - Included features: 
					
					#Opening files
					#vertical and horizontal scrolling
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					
0.0004 - Included features: 
					
					#Opening files
					#vertical and horizontal scrolling
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					
0.0005 - General Cleanup
					
					#General code cleanup
					
0.0006 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support

0.0007 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					
0.0008 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					
0.0009 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					
0.0010.00 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					#save file
					#save as file
					#status bar
					
0.0010.01 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					#save file
					#save as file
					#status bar
					
					IN DEVELOPMENT
					#runs current file as batch script (very basic running), doesn't actually do anything with the response it gets back yet
					
0.0010.02 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					#save file
					#save as file
					#status bar
					
					IN DEVELOPMENT
					#runs current file as batch script (very basic running), doesn't actually do anything with the response it gets back yet
					#Working on getting the window to hold the batch results up and running. Need to get a proper wndproc in order and might need to make a tweaked version of the TextView creation to fit the purpose of the window
					
0.0011.00 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					#save file
					#save as file
					#status bar
					#runs simple batch script, returns result and displays it in seperate window
					
					KINKS
					#The status bar and batch results window only appears when the whole window is resized, need to refresh the screen in a better place
					#the bactch result is tacking on two control characters at the end, figure out why 
					
0.0012.00 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					#save file
					#save as file
					#status bar
					#runs simple batch script, returns result and displays it in seperate window
					
					SOLVED ISSUES
					#The batch results window and status bar show up properly when activated now
					#A breakpoint only appears when clicked inside of the left margin
					
					KINKS
					#the batch result is tacking on two control characters at the end, figure out why 

0.0012.01 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					#save file
					#save as file
					#status bar
					#runs simple batch script, returns result and displays it in seperate window
					
					SOLVED ISSUES
					#The batch results window and status bar show up properly when activated now
					#A breakpoint only appears when clicked inside of the left margin
					#The output from a batch script no longer has two control characters at the end - had to add 2 to the buffer length or else it wouldn't print the last "\n"
					
					KINKS
					#
					
					TODO: 
					#Figure out next features
						- Most likely the batch command library implementation
						- Move onto syntax help after that?
						- Leave syntax highlighting until later?

0.0013.00 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					#save file
					#save as file
					#status bar
					#runs simple batch script, returns result and displays it in seperate window
					#Command library implemented
					
					SOLVED ISSUES
					#The batch results window and status bar show up properly when activated now
					#A breakpoint only appears when clicked inside of the left margin
					#The output from a batch script no longer has two control characters at the end - had to add 2 to the buffer length or else it wouldn't print the last "\n"
					
					KINKS
					#
					
					TODO: 
					#Get a transparent background on the check boxes
					#Figure out system of ensuring that the label/control width and height is appropriate for the concerned text or options
					#Figure out next features
						- Move onto syntax help after that?
						- Leave syntax highlighting until later?

0.0014.00 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					#save file
					#save as file
					#status bar
					#runs simple batch script, returns result and displays it in seperate window
					#Command library implemented
					#Command implementation completed
					
					SOLVED ISSUES
					#The batch results window and status bar show up properly when activated now
					#A breakpoint only appears when clicked inside of the left margin
					#The output from a batch script no longer has two control characters at the end - had to add 2 to the buffer length or else it wouldn't print the last "\n"
					
					KINKS
					#
					
					TODO: 
					#See if there is a way to sort out scrolling for the CMD window
					#Figure out next features
						- Move onto syntax help after that?
						- Leave syntax highlighting until later?
						
0.0015.00 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					#save file
					#save as file
					#status bar
					#runs simple batch script, returns result and displays it in seperate window
					#Command library implemented
					#Command implementation completed
					#Syntax hightlighting
					#Sytnax assistance (FOR and IF statements)
					#WiX installer included
					
					SOLVED ISSUES
					#The batch results window and status bar show up properly when activated now
					#A breakpoint only appears when clicked inside of the left margin
					#The output from a batch script no longer has two control characters at the end - had to add 2 to the buffer length or else it wouldn't print the last "\n"
					
					KINKS
					#
					
					TODO: 
					#See if there is a way to sort out scrolling for the CMD window
					#LAST NEW FEATURE : MENU BAR (ICONS)
					#Test installer

0.0016.00 - Included features: 

					#Opening files
					#vertical and horizontal scrolling 
					#enhanced painting
					#options dialog (font/colors)
					#mouse scrolling
					#margins
					#line numbers
					#breakpoints
					#long line highlighting
					#improved options
					#save options
					#unicode support
					#unicode text rendering
					#keyboard navigation
					#right click context menu
					#copy functionality
					#Text Editing
					#save file
					#save as file
					#status bar
					#runs simple batch script, returns result and displays it in seperate window
					#Command library implemented
					#Command implementation completed
					#Syntax hightlighting
					#Sytnax assistance (FOR and IF statements)
					#WiX installer included
					#Toolbar implemented (any more options?)
					
					SOLVED ISSUES
					#The batch results window and status bar show up properly when activated now
					#A breakpoint only appears when clicked inside of the left margin
					#The output from a batch script no longer has two control characters at the end - had to add 2 to the buffer length or else it wouldn't print the last "\n"
					
					KINKS
					#
					
					TODO: 
					#See if there is a way to sort out scrolling for the CMD window
					#Test installer