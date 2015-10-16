#!/usr/bin/perl

# Create the Hall C style map file for the SHMS shower counter
#
# Connected to FADC250s.  Assume one slot per column.  Signal cables
# are labled C-R, where C is [1:16] and R is [1:14].  The
# plane will be defined as 2 and the detector number as (C-1)*16+R

$ROC = 22;
@SLOTS = (5,6,7,8,9,10,13,14,15,16,17,18,19,20);
$PLANE = 2; 			# Call the shower array plane 2
$NROWS=16;

$SHOWER_ID=23;
$FILE="shower.map";

open(MAP,">$FILE");

print MAP "!SHMS\n";
print MAP "! SSHOWER_ID=$SHOWER_ID\n";
print MAP "\n";
print MAP "ROC=$ROC\n";
print MAP "nsubadd=16\n";
print MAP "mask=1\n";
print MAP "bsub=560\n\n";
print MAP "detector=$SHOWER_ID\n";

$column=1;
foreach $slot (@SLOTS) {
    print MAP "SLOT=$slot\n";
    for ($row=1;$row<=$NROWS;$row++) {
	$subadd = $row-1;
	$dno = ($column-1)*$NROWS+$row;
	print MAP "$subadd, $PLANE, $dno, 0 ! Row $row, Column $column\n";
    }
    $column++;
}
