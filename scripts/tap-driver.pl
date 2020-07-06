#! /usr/bin/env perl
# Copyright (C) 2011-2013 Free Software Foundation, Inc.
# Copyright (C) 2018 Red Hat, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# As a special exception to the GNU General Public License, if you
# distribute this file as part of a program that contains a
# configuration script generated by Autoconf, you may include it under
# the same distribution terms that you use for the rest of that program.

# ---------------------------------- #
#  Imports, static data, and setup.  #
# ---------------------------------- #

use warnings FATAL => 'all';
use strict;
use Getopt::Long ();
use TAP::Parser;
use Term::ANSIColor qw(:constants);

my $ME = "tap-driver.pl";
my $VERSION = "2018-11-30";

my $USAGE = <<'END';
Usage:
  tap-driver [--test-name=TEST] [--color={always|never|auto}]
             [--verbose] [--show-failures-only]
END

my $HELP = "$ME: TAP-aware test driver for QEMU testsuite harness." .
           "\n" . $USAGE;

# It's important that NO_PLAN evaluates "false" as a boolean.
use constant NO_PLAN => 0;
use constant EARLY_PLAN => 1;
use constant LATE_PLAN => 2;

use constant DIAG_STRING => "#";

# ------------------- #
#  Global variables.  #
# ------------------- #

my $testno = 0;     # Number of test results seen so far.
my $bailed_out = 0; # Whether a "Bail out!" directive has been seen.
my $failed = 0;     # Final exit code

# Whether the TAP plan has been seen or not, and if yes, which kind
# it is ("early" is seen before any test result, "late" otherwise).
my $plan_seen = NO_PLAN;

# ----------------- #
#  Option parsing.  #
# ----------------- #

my %cfg = (
  "color" => 0,
  "verbose" => 0,
  "show-failures-only" => 0,
);

my $color = "auto";
my $test_name = undef;

# Perl's Getopt::Long allows options to take optional arguments after a space.
# Prevent --color by itself from consuming other arguments
foreach (@ARGV) {
  if ($_ eq "--color" || $_ eq "-color") {
    $_ = "--color=$color";
  }
}

Getopt::Long::GetOptions
  (
    'help' => sub { print $HELP; exit 0; },
    'version' => sub { print "$ME $VERSION\n"; exit 0; },
    'test-name=s' => \$test_name,
    'color=s'  => \$color,
    'show-failures-only' => sub { $cfg{"show-failures-only"} = 1; },
    'verbose' => sub { $cfg{"verbose"} = 1; },
  ) or exit 1;

if ($color =~ /^always$/i) {
  $cfg{'color'} = 1;
} elsif ($color =~ /^never$/i) {
  $cfg{'color'} = 0;
} elsif ($color =~ /^auto$/i) {
  $cfg{'color'} = (-t STDOUT);
} else {
  die "Invalid color mode: $color\n";
}

# ------------- #
#  Prototypes.  #
# ------------- #

sub colored ($$);
sub decorate_result ($);
sub extract_tap_comment ($);
sub handle_tap_bailout ($);
sub handle_tap_plan ($);
sub handle_tap_result ($);
sub is_null_string ($);
sub main ();
sub report ($;$);
sub stringify_result_obj ($);
sub testsuite_error ($);

# -------------- #
#  Subroutines.  #
# -------------- #

# If the given string is undefined or empty, return true, otherwise
# return false.  This function is useful to avoid pitfalls like:
#   if ($message) { print "$message\n"; }
# which wouldn't print anything if $message is the literal "0".
sub is_null_string ($)
{
  my $str = shift;
  return ! (defined $str and length $str);
}

sub stringify_result_obj ($)
{
  my $result_obj = shift;
  if ($result_obj->is_unplanned || $result_obj->number != $testno)
    {
      return "ERROR";
    }
  elsif ($plan_seen == LATE_PLAN)
    {
      return "ERROR";
    }
  elsif (!$result_obj->directive)
    {
      return $result_obj->is_ok ? "PASS" : "FAIL";
    }
  elsif ($result_obj->has_todo)
    {
      return $result_obj->is_actual_ok ? "XPASS" : "XFAIL";
    }
  elsif ($result_obj->has_skip)
    {
      return $result_obj->is_ok ? "SKIP" : "FAIL";
    }
  die "$ME: INTERNAL ERROR"; # NOTREACHED
}

sub colored ($$)
{
  my ($color_string, $text) = @_;
  return $color_string . $text . RESET;
}

sub decorate_result ($)
{
  my $result = shift;
  return $result unless $cfg{"color"};
  my %color_for_result =
    (
      "ERROR" => BOLD.MAGENTA,
      "PASS"  => GREEN,
      "XPASS" => BOLD.YELLOW,
      "FAIL"  => BOLD.RED,
      "XFAIL" => YELLOW,
      "SKIP"  => BLUE,
    );
  if (my $color = $color_for_result{$result})
    {
      return colored ($color, $result);
    }
  else
    {
      return $result; # Don't colorize unknown stuff.
    }
}

sub report ($;$)
{
  my ($msg, $result, $explanation) = (undef, @_);
  if ($result =~ /^(?:X?(?:PASS|FAIL)|SKIP|ERROR)/)
    {
      # Output on console might be colorized.
      $msg = decorate_result($result);
      if ($result =~ /^(?:PASS|XFAIL|SKIP)/)
        {
          return if $cfg{"show-failures-only"};
        }
      else
        {
          $failed = 1;
        }
    }
  elsif ($result eq "#")
    {
      $msg = "  ";
    }
  else
    {
      die "$ME: INTERNAL ERROR"; # NOTREACHED
    }
  $msg .= " $explanation" if defined $explanation;
  print $msg . "\n";
}

sub testsuite_error ($)
{
  report "ERROR", "$test_name - $_[0]";
}

sub handle_tap_result ($)
{
  $testno++;
  my $result_obj = shift;

  my $test_result = stringify_result_obj $result_obj;
  my $string = $result_obj->number;

  my $description = $result_obj->description;
  $string .= " $test_name" unless is_null_string $test_name;
  $string .= " $description" unless is_null_string $description;

  if ($plan_seen == LATE_PLAN)
    {
      $string .= " # AFTER LATE PLAN";
    }
  elsif ($result_obj->is_unplanned)
    {
      $string .= " # UNPLANNED";
    }
  elsif ($result_obj->number != $testno)
    {
      $string .= " # OUT-OF-ORDER (expecting $testno)";
    }
  elsif (my $directive = $result_obj->directive)
    {
      $string .= " # $directive";
      my $explanation = $result_obj->explanation;
      $string .= " $explanation"
        unless is_null_string $explanation;
    }

  report $test_result, $string;
}

sub handle_tap_plan ($)
{
  my $plan = shift;
  if ($plan_seen)
    {
      # Error, only one plan per stream is acceptable.
      testsuite_error "multiple test plans";
      return;
    }
  # The TAP plan can come before or after *all* the TAP results; we speak
  # respectively of an "early" or a "late" plan.  If we see the plan line
  # after at least one TAP result has been seen, assume we have a late
  # plan; in this case, any further test result seen after the plan will
  # be flagged as an error.
  $plan_seen = ($testno >= 1 ? LATE_PLAN : EARLY_PLAN);
  # If $testno > 0, we have an error ("too many tests run") that will be
  # automatically dealt with later, so don't worry about it here.  If
  # $plan_seen is true, we have an error due to a repeated plan, and that
  # has already been dealt with above.  Otherwise, we have a valid "plan
  # with SKIP" specification, and should report it as a particular kind
  # of SKIP result.
  if ($plan->directive && $testno == 0)
    {
      my $explanation = is_null_string ($plan->explanation) ?
                        undef : "- " . $plan->explanation;
      report "SKIP", $explanation;
    }
}

sub handle_tap_bailout ($)
{
  my ($bailout, $msg) = ($_[0], "Bail out!");
  $bailed_out = 1;
  $msg .= " " . $bailout->explanation
    unless is_null_string $bailout->explanation;
  testsuite_error $msg;
}

sub extract_tap_comment ($)
{
  my $line = shift;
  if (index ($line, DIAG_STRING) == 0)
    {
      # Strip leading `DIAG_STRING' from `$line'.
      $line = substr ($line, length (DIAG_STRING));
      # And strip any leading and trailing whitespace left.
      $line =~ s/(?:^\s*|\s*$)//g;
      # Return what is left (if any).
      return $line;
    }
  return "";
}

sub main ()
{
  my $iterator = TAP::Parser::Iterator::Stream->new(\*STDIN);
  my $parser = TAP::Parser->new ({iterator => $iterator });

  STDOUT->autoflush(1);
  while (defined (my $cur = $parser->next))
    {
      # Parsing of TAP input should stop after a "Bail out!" directive.
      next if $bailed_out;

      if ($cur->is_plan)
        {
          handle_tap_plan ($cur);
        }
      elsif ($cur->is_test)
        {
          handle_tap_result ($cur);
        }
      elsif ($cur->is_bailout)
        {
          handle_tap_bailout ($cur);
        }
      elsif ($cfg{"verbose"})
        {
          my $comment = extract_tap_comment ($cur->raw);
          report "#", "$comment" if length $comment;
       }
    }
  # A "Bail out!" directive should cause us to ignore any following TAP
  # error.
  if (!$bailed_out)
    {
      if (!$plan_seen)
        {
          testsuite_error "missing test plan";
        }
      elsif ($parser->tests_planned != $parser->tests_run)
        {
          my ($planned, $run) = ($parser->tests_planned, $parser->tests_run);
          my $bad_amount = $run > $planned ? "many" : "few";
          testsuite_error (sprintf "too %s tests run (expected %d, got %d)",
                                   $bad_amount, $planned, $run);
        }
    }
}

# ----------- #
#  Main code. #
# ----------- #

main;
exit($failed);

# Local Variables:
# perl-indent-level: 2
# perl-continued-statement-offset: 2
# perl-continued-brace-offset: 0
# perl-brace-offset: 0
# perl-brace-imaginary-offset: 0
# perl-label-offset: -2
# cperl-indent-level: 2
# cperl-brace-offset: 0
# cperl-continued-brace-offset: 0
# cperl-label-offset: -2
# cperl-extra-newline-before-brace: t
# cperl-merge-trailing-else: nil
# cperl-continued-statement-offset: 2
# End:
