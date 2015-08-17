-------------------------------------------------------------
16/08/2014 (v0.0004)

When going into the Options dialog, selecting "Long Lines" in the "Display items" list triggers an exception (Access Violation Reading Locaiton). It does so intermittently

CURRENT WORK IN PROGRESS:

Problem source: UpdatePreviewPane()

The LOWORD of the data that it's getting from the LB_GETITEMDATA from IDC_ITEMLIST is coming back as an incredibly high number, hence going out of the 32 slot array of g_rgbTempColourList[]

Why does this happen just for the LOWORD of this particular list item?

RESOLUTION: When calling the AddColourListItem() batch, the Long Line Text parameter was given -1, instead of the correct value TXC_LONGLINETEXT
-------------------------------------------------------------

