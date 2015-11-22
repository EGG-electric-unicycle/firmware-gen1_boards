#!/usr/bin/perl
# NOTE: needs libnet-telnet-perl package.
use Net::Telnet;
use Cwd 'abs_path';
 
my $numArgs = $#ARGV + 1;
if($numArgs != 1) {
    die( "Usage ./stm32_flash.pl [main.bin] \n");
}

my $file = abs_path($ARGV[0]);

my $ip = "127.0.0.1";   # localhost
my $port = 4444;

my $telnet = new Net::Telnet (
    Port   => $port,
    Timeout=> 30,
    Errmode=> 'die',
    Prompt => '/>/');

$telnet->open($ip);

#print $telnet->cmd('reset halt');
#print $telnet->cmd('flash probe 0');
#print $telnet->cmd('flash write_image erase '.$file.' 0x08000000');
#print $telnet->cmd('reset');
#print $telnet->cmd('exit');

print $telnet->cmd('init');
print $telnet->cmd('halt');
print $telnet->cmd('flash write_image erase '.$file.' 0x08000000');
print $telnet->cmd('reset');
print $telnet->cmd('exit');


print "\n";
