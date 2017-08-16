#!/usr/bin/perl

# Project: Si4463 Radio Library for AVR and Arduino (WDS Radio config processor)
# Author: Zak Kemble, contact@zakkemble.co.uk
# Copyright: (C) 2017 by Zak Kemble
# License: GNU GPL v3 (see License.txt)
# Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/

# This script takes a generated radio configuration header file from WDS and removes various property
# groups and fields that the configuration applies, but are actually the same as their default values.
# This script generally reduces the config size by around 20-40%.
# The config will still contain unnecessary settings, like various antenna diversity parameters even though
# antenna diversity is disabled.

# Not fully tested, but seems to work fine when I've used it

# Usage:
# radio_config.pl <in file> [out file (default is radio_config.h, - (just a dash) will output to stdout)]

use warnings;
use strict;
use File::Basename;
#use Data::Dumper;

my @props;
my @groupNames;

# GLOBAL
$groupNames[0x00] = 'GLOBAL';
$props[0x00][0x00] = 0x40;
$props[0x00][0x01] = 0x00;
$props[0x00][0x02] = 0x18;
$props[0x00][0x03] = 0x20;
$props[0x00][0x04] = 0x00;
$props[0x00][0x05] = 0x00;
$props[0x00][0x06] = 0x01;
$props[0x00][0x07] = 0x60;
$props[0x00][0x08] = 0x00;
$props[0x00][0x09] = 0x00;

# INT_CTL
$groupNames[0x01] = 'INT_CTL';
$props[0x01][0x00] = 0x04;
$props[0x01][0x01] = 0x00;
$props[0x01][0x02] = 0x00;
$props[0x01][0x03] = 0x04;

# FRR_CTL
$groupNames[0x02] = 'FRR_CTL';
$props[0x02][0x00] = 0x01;
$props[0x02][0x01] = 0x02;
$props[0x02][0x02] = 0x09;
$props[0x02][0x03] = 0x00;

# PREAMBLE
$groupNames[0x10] = 'PREAMBLE';
$props[0x10][0x00] = 0x08;
$props[0x10][0x01] = 0x14;
$props[0x10][0x02] = 0x00;
$props[0x10][0x03] = 0x0f;
$props[0x10][0x04] = 0x21;
$props[0x10][0x05] = 0x00;
$props[0x10][0x06] = 0x00;
$props[0x10][0x07] = 0x00;
$props[0x10][0x08] = 0x00;
$props[0x10][0x09] = 0x00;
$props[0x10][0x0a] = 0x00;
$props[0x10][0x0b] = 0x00;
$props[0x10][0x0c] = 0x00;
$props[0x10][0x0d] = 0x00;

# SYNC
$groupNames[0x11] = 'SYNC';
$props[0x11][0x00] = 0x01;
$props[0x11][0x01] = 0x2d;
$props[0x11][0x02] = 0xd4;
$props[0x11][0x03] = 0x2d;
$props[0x11][0x04] = 0xd4;
$props[0x11][0x05] = 0x00;

# PKT
$groupNames[0x12] = 'PKT';
$props[0x12][0x00] = 0x00;
$props[0x12][0x01] = 0x01;
$props[0x12][0x02] = 0x08;
$props[0x12][0x03] = 0xff;
$props[0x12][0x04] = 0xff;
$props[0x12][0x05] = 0x00;
$props[0x12][0x06] = 0x00;
$props[0x12][0x07] = 0x00;
$props[0x12][0x08] = 0x00;
$props[0x12][0x09] = 0x00;
$props[0x12][0x0a] = 0x00;
$props[0x12][0x0b] = 0x30;
$props[0x12][0x0c] = 0x30;
$props[0x12][0x0d] = 0x00;
$props[0x12][0x0e] = 0x00;
$props[0x12][0x0f] = 0x00;
$props[0x12][0x10] = 0x00;
$props[0x12][0x11] = 0x00;
$props[0x12][0x12] = 0x00;
$props[0x12][0x13] = 0x00;
$props[0x12][0x14] = 0x00;
$props[0x12][0x15] = 0x00;
$props[0x12][0x16] = 0x00;
$props[0x12][0x17] = 0x00;
$props[0x12][0x18] = 0x00;
$props[0x12][0x19] = 0x00;
$props[0x12][0x1a] = 0x00;
$props[0x12][0x1b] = 0x00;
$props[0x12][0x1c] = 0x00;
$props[0x12][0x1d] = 0x00;
$props[0x12][0x1e] = 0x00;
$props[0x12][0x1f] = 0x00;
$props[0x12][0x20] = 0x00;
$props[0x12][0x21] = 0x00;
$props[0x12][0x22] = 0x00;
$props[0x12][0x23] = 0x00;
$props[0x12][0x24] = 0x00;
$props[0x12][0x25] = 0x00;
$props[0x12][0x26] = 0x00;
$props[0x12][0x27] = 0x00;
$props[0x12][0x28] = 0x00;
$props[0x12][0x29] = 0x00;
$props[0x12][0x2a] = 0x00;
$props[0x12][0x2b] = 0x00;
$props[0x12][0x2c] = 0x00;
$props[0x12][0x2d] = 0x00;
$props[0x12][0x2e] = 0x00;
$props[0x12][0x2f] = 0x00;
$props[0x12][0x30] = 0x00;
$props[0x12][0x31] = 0x00;
$props[0x12][0x32] = 0x00;
$props[0x12][0x33] = 0x00;
$props[0x12][0x34] = 0x00;
$props[0x12][0x35] = 0x00;
$props[0x12][0x36] = 0x00;
$props[0x12][0x37] = 0x00;
$props[0x12][0x38] = 0x00;
$props[0x12][0x39] = 0x00;

# MODEM
$groupNames[0x20] = 'MODEM';
$props[0x20][0x00] = 0x02;
$props[0x20][0x01] = 0x80;
$props[0x20][0x02] = 0x07;
$props[0x20][0x03] = 0x0f;
$props[0x20][0x04] = 0x42;
$props[0x20][0x05] = 0x40;
$props[0x20][0x06] = 0x01;
$props[0x20][0x07] = 0xc9;
$props[0x20][0x08] = 0xc3;
$props[0x20][0x09] = 0x80;
$props[0x20][0x0a] = 0x00;
$props[0x20][0x0b] = 0x06;
$props[0x20][0x0c] = 0xd3;
$props[0x20][0x0d] = 0x00;
$props[0x20][0x0e] = 0x00;
$props[0x20][0x0f] = 0x67;
$props[0x20][0x10] = 0x60;
$props[0x20][0x11] = 0x4d;
$props[0x20][0x12] = 0x36;
$props[0x20][0x13] = 0x21;
$props[0x20][0x14] = 0x11;
$props[0x20][0x15] = 0x08;
$props[0x20][0x16] = 0x03;
$props[0x20][0x17] = 0x01;
$props[0x20][0x18] = 0x01;
$props[0x20][0x19] = 0x00;
$props[0x20][0x1a] = 0x08;
$props[0x20][0x1b] = 0x03;
$props[0x20][0x1c] = 0xc0;
$props[0x20][0x1d] = 0x00;
$props[0x20][0x1e] = 0x10;
$props[0x20][0x1f] = 0x20;
$props[0x20][0x20] = 0x00;
$props[0x20][0x21] = 0xe8;
$props[0x20][0x22] = 0x00;
$props[0x20][0x23] = 0x4b;
$props[0x20][0x24] = 0x06;
$props[0x20][0x25] = 0xd3;
$props[0x20][0x26] = 0xa0;
$props[0x20][0x27] = 0x06;
$props[0x20][0x28] = 0xd3;
$props[0x20][0x29] = 0x02;
$props[0x20][0x2a] = 0xc0;
$props[0x20][0x2b] = 0x00;
$props[0x20][0x2c] = 0x00;
$props[0x20][0x2d] = 0x23;
$props[0x20][0x2e] = 0x83;
$props[0x20][0x2f] = 0x69;
$props[0x20][0x30] = 0x00;
$props[0x20][0x31] = 0x40;
$props[0x20][0x32] = 0xa0;
$props[0x20][0x33] = 0x00;
$props[0x20][0x34] = 0x00;
$props[0x20][0x35] = 0xe0;
$props[0x20][0x36] = undef;
$props[0x20][0x37] = undef;
$props[0x20][0x38] = 0x11;
$props[0x20][0x39] = 0x10;
$props[0x20][0x3a] = 0x10;
$props[0x20][0x3b] = 0x0b;
$props[0x20][0x3c] = 0x1c;
$props[0x20][0x3d] = 0x40;
$props[0x20][0x3e] = 0x00;
$props[0x20][0x3f] = 0x00;
$props[0x20][0x40] = 0x2b;
$props[0x20][0x41] = 0x0c;
$props[0x20][0x42] = 0xa4;
$props[0x20][0x43] = 0x03;
$props[0x20][0x44] = undef;
$props[0x20][0x45] = 0x02;
$props[0x20][0x46] = 0x00;
$props[0x20][0x47] = 0xa3;
$props[0x20][0x48] = 0x02;
$props[0x20][0x49] = 0x80;
$props[0x20][0x4a] = 0xff;
$props[0x20][0x4b] = 0x0c;
$props[0x20][0x4c] = 0x01;
$props[0x20][0x4d] = 0x00;
$props[0x20][0x4e] = 0x40;
$props[0x20][0x4f] = undef;
$props[0x20][0x50] = 0x00;
$props[0x20][0x51] = 0x08;
$props[0x20][0x52] = undef;
$props[0x20][0x53] = undef;
$props[0x20][0x54] = 0x00;
$props[0x20][0x55] = 0x00;
$props[0x20][0x56] = 0xff;
$props[0x20][0x57] = 0x00;
$props[0x20][0x58] = 0x00;
$props[0x20][0x59] = 0x00;
$props[0x20][0x5a] = 0x00;
$props[0x20][0x5b] = 0x00;
$props[0x20][0x5c] = 0x00;
$props[0x20][0x5d] = 0x00;
$props[0x20][0x5e] = 0x00;
$props[0x20][0x5f] = 0x00;

# MODEM_CHFLT
$groupNames[0x21] = 'MODEM_CHFLT';
$props[0x21][0x00] = 0xff;
$props[0x21][0x01] = 0xba;
$props[0x21][0x02] = 0x0f;
$props[0x21][0x03] = 0x51;
$props[0x21][0x04] = 0xcf;
$props[0x21][0x05] = 0xa9;
$props[0x21][0x06] = 0xc9;
$props[0x21][0x07] = 0xfc;
$props[0x21][0x08] = 0x1b;
$props[0x21][0x09] = 0x1e;
$props[0x21][0x0a] = 0x0f;
$props[0x21][0x0b] = 0x01;
$props[0x21][0x0c] = 0xfc;
$props[0x21][0x0d] = 0xfd;
$props[0x21][0x0e] = 0x15;
$props[0x21][0x0f] = 0xff;
$props[0x21][0x10] = 0x00;
$props[0x21][0x11] = 0x0f;
$props[0x21][0x12] = 0xff;
$props[0x21][0x13] = 0xc4;
$props[0x21][0x14] = 0x30;
$props[0x21][0x15] = 0x7f;
$props[0x21][0x16] = 0xf5;
$props[0x21][0x17] = 0xb5;
$props[0x21][0x18] = 0xb8;
$props[0x21][0x19] = 0xde;
$props[0x21][0x1a] = 0x05;
$props[0x21][0x1b] = 0x17;
$props[0x21][0x1c] = 0x16;
$props[0x21][0x1d] = 0x0c;
$props[0x21][0x1e] = 0x03;
$props[0x21][0x1f] = 0x00;
$props[0x21][0x20] = 0x15;
$props[0x21][0x21] = 0xff;
$props[0x21][0x22] = 0x00;
$props[0x21][0x23] = 0x00;

# PA
$groupNames[0x22] = 'PA';
$props[0x22][0x00] = 0x08;
$props[0x22][0x01] = 0x7f;
$props[0x22][0x02] = 0x00;
$props[0x22][0x03] = 0x5d;
$props[0x22][0x04] = 0x80;
$props[0x22][0x05] = 0x23;
$props[0x22][0x06] = 0x03;

# SYNTH
$groupNames[0x23] = 'SYNTH';
$props[0x23][0x00] = 0x2c;
$props[0x23][0x01] = 0x0e;
$props[0x23][0x02] = 0x0b;
$props[0x23][0x03] = 0x04;
$props[0x23][0x04] = 0x0c;
$props[0x23][0x05] = 0x73;
$props[0x23][0x06] = 0x03;
$props[0x23][0x07] = 0x05;

# MATCH
$groupNames[0x30] = 'MATCH';
$props[0x30][0x00] = 0x00;
$props[0x30][0x01] = 0x00;
$props[0x30][0x02] = 0x00;
$props[0x30][0x03] = 0x00;
$props[0x30][0x04] = 0x00;
$props[0x30][0x05] = 0x00;
$props[0x30][0x06] = 0x00;
$props[0x30][0x07] = 0x00;
$props[0x30][0x08] = 0x00;
$props[0x30][0x09] = 0x00;
$props[0x30][0x0a] = 0x00;
$props[0x30][0x0b] = 0x00;

# FREQ_CONTROL
$groupNames[0x40] = 'FREQ_CONTROL';
$props[0x40][0x00] = 0x3c;
$props[0x40][0x01] = 0x08;
$props[0x40][0x02] = 0x00;
$props[0x40][0x03] = 0x00;
$props[0x40][0x04] = 0x00;
$props[0x40][0x05] = 0x00;
$props[0x40][0x06] = 0x20;
$props[0x40][0x07] = 0xff;

# RX_HOP
# TODO

# PTI
$groupNames[0xf0] = 'PTI';
$props[0xf0][0x00] = 0x80;
$props[0xf0][0x01] = 0x13;
$props[0xf0][0x02] = 0x88;
$props[0xf0][0x03] = 0x00;

print "// Si446x WDS Radio configuration header processor (C) 2017 by Zak Kemble\n";

my $num_args = @ARGV;
if ($num_args != 1 && $num_args != 2)
{
	print "\nUsage: " . basename($0) . " <in file> [out file (default is radio_config.h, - (just a dash) will output to stdout)]\n";
	exit 1;
}

my $infile = $ARGV[0];
if(!-f $infile)
{
	print "\nError: Input file not found\n";
	exit 1;
}

my $outfile = $ARGV[1];

# Read lines to array
open my $fh, '<', $infile;
chomp(my @lines = <$fh>);
close $fh;

my $afcEnabled = 0;
foreach(@lines)
{
	if($_ =~ /AFC_en: ([0-9]+)/)
	{
		$afcEnabled = 1 if($1 eq '1');
		print "// AFC: $1\n";
		last;
	}
}

my %manualProps;

# Set FIFO_MODE in GLOBAL_CONFIG (use single 129 byte FIFO)
$manualProps{RF_CUSTOM_1} = {
#	applyToAll => 1,
	group => 0x00,
	index => 0x03,
	type => 'or', # Bitwise OR
	value => 16
};

# Set packet length to 128 (PKT_FIELD_2_LENGTH)
$manualProps{RF_CUSTOM_2} = {
#	applyToAll => 1,
	group => 0x12,
	index => 0x12,
	type => 'set',
	value => 0x80
};

if($afcEnabled)
{
	# WDS 3.2.10.0 and newer generate a config with AFC disabled even if it was ticked (MODEM_AFC_GAIN)
	$manualProps{RF_CUSTOM_3} = {
	#	applyToAll => 1,
		group => 0x20,
		index => 0x2E,
		type => 'set',
		value => 0xC6
	};

	$manualProps{RF_CUSTOM_4} = {
	#	applyToAll => 1,
		group => 0x20,
		index => 0x2F,
		type => 'set',
		value => 0xD4
	};
}

my %defines;
my @cfgOrder;

# Convert array of values to array of hashes
processProps(\@props);

# Parse the header file lines
getDefinesAndCfgOrder(\@lines, \%defines, \@cfgOrder);

my @propertyGroups;
my @currentProps = @props;

foreach(@cfgOrder)
{
	next if !defined($defines{$_}); # Missing #define
	
	if($defines{$_}->{isProperty})
	{
		my $group = $defines{$_}->{group};
		my $start = $defines{$_}->{start};
		my $data = $defines{$_}->{data};
		my $len = @{$data};
		
		next if !defined($currentProps[$group]); # Unknown group
		next if $start > @{$currentProps[$group]}; # Start is outside of known group boundary
		next if $start + $len > @{$currentProps[$group]}; # End is outside of known group boundary
		
		for(my $i=0;$i<$len;$i++)
		{
			$currentProps[$group][$i + $start]{new} = @{$data}[$i];
		}

		# Apply manual property settings
		# TODO what if property hasnt been defined in the input config? Will need to add it
		foreach(keys %manualProps)
		{
			if($manualProps{$_}{group} == $group)
			{
				my $prop = $currentProps[$group][$manualProps{$_}{index}];
				if($manualProps{$_}{type} eq 'or')
				{
					($prop->{new} = $prop->{original}) if !defined($prop->{new});
					$prop->{new} |= $manualProps{$_}{value};
				}
				else
				{
					$prop->{new} = $manualProps{$_}{value};
				}
			}
		}
	}
	else
	{
		# Copy 2D array
		my @a;
		foreach(@currentProps)
		{
			if(!defined($_))
			{
				push @a, undef;
			}
			else
			{
				push @a, [@{$_}];
			}
		}

		push @propertyGroups, [@a];
		push @propertyGroups, [$defines{$_}->{cmd}, @{$defines{$_}->{data}}];
		processProps(\@currentProps);
	}
}

push @propertyGroups, [@currentProps];

# TODO
#open my $fh22, '>', "a2.txt";
#print $fh22 Dumper(\@propertyGroups);
#close $fh22;

my @parts;
my $section = 0;

sub savePart
{
	my $things = shift;
	
	return if(@{$things->{values}} <= 0);

	splice(@{$things->{values}}, -$things->{gapSize}) if($things->{gapSize} > 0); # Remove any default values from the end

	my $len = @{$things->{values}};
	my %tmp = (
		isProperty => 1,
		group => $things->{group},
		len => $len,
		start => $things->{startIdx},
		data => [@{$things->{values}}],
		section => $section,
		part => $things->{partCount}
	);
	push @parts, \%tmp;

	$things->{partCount}++;
	$things->{startIdx} = 0;
	$things->{gapSize} = 0;
	$things->{lookingForStart} = 1;
	@{$things->{values}} = ();
}

foreach(@propertyGroups)
{
	next if(@{$_} < 1); # Empty
	
	if(ref(@{$_}[0]) ne 'ARRAY') # Is a command
	{
		my $name = @{$_}[0];

		my %tmp = (
			cmd => $name,
			data => [splice(@{$_}, 1)]
		);
		push @parts, \%tmp;
		
		$section++;
		
		next;
	}
	
	for(my $g=0;$g<@{$_};$g++) # Loop groups
	{
		next if !defined(@{$_}[$g]); # Not all group IDs exist
		
		my $group = @{$_}[$g];
		
		my %things = (
			group => $g,
			lookingForStart => 1,
			startIdx => 0,
			gapSize => 0,
			partCount => 0,
			values => []
		);

		for(my $p=0;$p<@{$group};$p++) # Loop properties
		{
			if(!defined(@{$group}[$p])) # Undefined property, some property IDs don't exist
			{
				savePart(\%things); # Start new part if we've got some values
				next;
			}
	
			my $property = @{$group}[$p];
			
			if($things{lookingForStart})
			{
				if(defined($property->{new}) && $property->{new} != $property->{original})
				{
					$things{lookingForStart} = 0;
					$things{startIdx} = $p;
				}
			}

			if(!$things{lookingForStart})
			{
				push @{$things{values}}, defined($property->{new}) ? $property->{new} : $property->{original};

				if(!defined($property->{new}) || $property->{new} == $property->{original}) # Is a default value
				{
					$things{gapSize}++;
					savePart(\%things) if($things{gapSize} >= 5); # Start a new part if 5 or more defaults in a row
				}
				else
				{
					$things{gapSize} = 0;
				}

				savePart(\%things) if(@{$things{values}} >= 12); # Max 12 values per part
			}
		}

		savePart(\%things); # Might have some values left after looping the properties
	}
}

# TODO
#open $fh22, '>', "a3.txt";
#print $fh22 Dumper(\@parts);
#close $fh22;

my $byteCount = 0;
my $outputDefines = '';
my $outputConfig = "#define RADIO_CONFIGURATION_DATA_ARRAY { \\\n";
foreach (@parts)
{
	$byteCount += @{$_->{data}};
	
	if($_->{isProperty})
	{
		$byteCount += 5;
		
		$outputDefines .= sprintf('#define %s_%d_%d 0x11, 0x%0.2X, 0x%0.2X, 0x%0.2X, %s',
			$groupNames[$_->{group}],
			$_->{section},
			$_->{part},
			$_->{group},
			$_->{len},
			$_->{start},
			join(', ', map { sprintf '0x%0.2X', $_ } @{$_->{data}})
		);
		
		$outputConfig .= sprintf('0x%0.2X, %s_%d_%d, \\',
			$_->{len} + 4,
			$groupNames[$_->{group}],
			$_->{section},
			$_->{part}
		);
	}
	else
	{
		$byteCount += 1;

		# Dont set the PATCH thing for POWER_UP
		@{$_->{data}}[1] = 1 if($_->{cmd} eq 'RF_POWER_UP');

		$outputDefines .= sprintf('#define %s %s',
			$_->{cmd},
			join(', ', map { sprintf "0x%0.2X", $_ } @{$_->{data}})
		);

		$outputConfig .= sprintf('0x%0.2X, %s, \\',
			@{$_->{data}} + 0,
			$_->{cmd}
		);

		# TODO after IR_CAL put into SPI_ACTIVE mode and clear interrupts
	}

	$outputDefines .= "\n";
	$outputConfig .= "\n";
}


print "// New byte count: $byteCount\n";
#printf("From %d bytes to %d bytes, %d%%\n", $originalByteCount, $byteCount, ($byteCount / $originalByteCount) * 100);

$outputConfig .= '}';

$outfile = 'radio_config.h' if(!defined($outfile) || !length($outfile));

my $stdout = 0;
if($outfile eq '-')
{
	$stdout = 1;
	$fh = *STDOUT;
}
else
{
	print "// Writing to $outfile\n";
	open $fh, '>', $outfile;
}

print $fh "\n#ifndef RADIO_CONFIG_H_\n";
print $fh "#define RADIO_CONFIG_H_\n\n";
print $fh $outputDefines;
print $fh "\n";
print $fh $outputConfig;
print $fh "\n\n#endif\n";

if(!$stdout){close $fh;}

exit;

sub processProps
{
	my $props = shift;

	for(my $g=0;$g<@{$props};$g++)
	{
		my $group = $g;

		next if !defined(@{$props}[$group]);

		for(my $i=0;$i<@{@{$props}[$group]};$i++)
		{
			my $property = $i;
			
			next if !defined(@{@{$props}[$group]}[$property]);
			
			my $prop = @{@{$props}[$group]}[$property];
			
			if(ref($prop) eq 'HASH') # If its a hash then copy new to original and clear new
			{
				my %tmp = (
					original => $prop->{original},
					new => undef
				);
				
				$tmp{original} = $prop->{new} if defined($prop->{new});
				
				@{@{$props}[$group]}[$property] = \%tmp;
			}
			else # Convert to hash
			{
				my %tmp = (
					original => $prop,
					new => undef
				);
				@{@{$props}[$group]}[$property] = \%tmp;
			}
		}
	}
}

sub getDefinesAndCfgOrder
{
	my $lines = shift;
	my $defines = shift;
	my $cfgOrder = shift;
	
	my $originalByteCount = 0;
	
	foreach (@{$lines})
	{
		# TODO what if the define is commented out? or has spaces/tabs infront? block comment?

		if($_ =~ /\s*0x[A-Za-z0-9]+\s*,\s*(RF_[A-Za-z0-9_-]+)/) # Save the define name into the config order list
		{
			push @{$cfgOrder}, $1;
		}
		elsif(index($_, '#define RF_') != -1) # This is the actual #define line
		{
			$_ =~ s/,//g; # Remove commas
			my @values = split(' ', $_); # Split by space
			my $name = $values[1];
			splice(@values, 0, 2);

			# Hex string to int
			foreach(@values)
			{
				$_ = hex($_);
			}
			
			my $isProperty = ($values[0] == 0x11);

			$originalByteCount += @values + 1;
			
			if($isProperty)
			{
				%{$defines->{$name}} = (
					isProperty => 1,
					group => $values[1],
					len => $values[2], # TODO needed?
					start => $values[3],
					data => []
				);
				splice(@values, 0, 4);
				$defines->{$name}{data} = \@values;
			}
			else
			{
				%{$defines->{$name}} = (
					isProperty => 0,
					cmd => $name,
					data => []
				);
				$defines->{$name}{data} = \@values;
			}
		}
	}
	
	#print Dumper($defines);
	
	print "// Original byte count: $originalByteCount\n";
}
