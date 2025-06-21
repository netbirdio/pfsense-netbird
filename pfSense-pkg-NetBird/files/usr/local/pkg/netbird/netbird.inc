<?php

require_once('service-utils.inc');
require_once('vendor/autoload.php');

use Nette\Utils\Html;

define('NETBIRD_BIN', '/usr/local/bin/netbird');
define('PKG_BIN', '/usr/sbin/pkg');

function netbird_get_pkg_info($field_map = NULL, $packages = NULL)
{
    $pkg_delm = "\t";

    $cmd = implode(' ', [PKG_BIN, 'query', escapeshellarg(implode($pkg_delm, $field_map)), implode(' ', $packages)]);

    exec($cmd, $out, $rc);

    if ($rc !== 0)
        return false;

    $return_keys = array_values(array_flip($field_map));
    foreach ($out as $row_idx => $row_string)
        foreach (explode($pkg_delm, $row_string) as $field_idx => $field_string)
            $ret[$row_idx][$return_keys[$field_idx]] = $field_string;

    return $ret;
}


function netbird_is_connected()
{
    if ($status = netbird_get_status()){
        return $status["management"]["connected"];
    }
    return false;
}

function netbird_is_running()
{
    return is_service_running('netbird');
}

function netbird_get_status()
{
    $cmd = implode(' ', [NETBIRD_BIN, 'status', '--json']);
    exec($cmd, $out, $rc);

    if ($rc !==0 || empty($out)) {
        return false;
    }

    $status = json_decode(implode('', $out), true);
    if (json_last_error() !== JSON_ERROR_NONE || !is_array($status)) {
        return false;
    }
    return $status;
}

function netbird_connection_info()
{
    if (!netbird_is_running()) {
        $info = gettext('NetBird is not running.');
        $type = 'danger';
    } elseif (!netbird_is_connected()) {
        $info = gettext('NetBird is not connected. Refresh or check the NetBird status page.');
        $type = 'warning';
    } else {
        $info = gettext('NetBird is connected.');
        $type = 'info';
        $closeable = true;
    }

    print_info_box($info, $type, $closeable ? 'close' : false);
}

function netbird_render_status_html_table(string $name, string $summary): string
{
    $table = Html::el('table')
        ->class('table table-hover table-condensed')->addHtml(
            Html::el('tbody')->addHtml(
                Html::el('tr')->addHtml(
                    Html::el('td')->addHtml(
                        Html::el('h5')->addHtml(
                            Html::el('a')->setText($name)
                        )
                    )
                )
            )->addHtml(Html::el('tr')->addHtml(
                Html::el('td')->addHtml(
                    Html::el('pre')->addAttributes([
                        'style' => 'max-height: 500px; overflow-y: auto;'
                    ])->setText($summary)
                )
            ))
        );

    return $table->toHtml();
}

function netbird_display_peer_connection_status()
{
    $summary = netbird_get_peer_connection_status();
    print(netbird_render_status_html_table('Connection Status', $summary));
}

function netbird_display_peers_details_status()
{
    $status = netbird_get_status();
    $peers = $status['peers'] ?? [];
    $summary = netbird_get_peers_detail($peers, $status['quantumResistance'], $status['quantumResistancePermissive']);
    print(netbird_render_status_html_table('Peers Detail', $summary));
}


function netbird_get_peer_connection_status(): string
{
    $status = netbird_get_status();

    // Management
    $management = $status['management'] ?? [];
    if (!empty($management['connected'])) {
        $managementStr = "Connected";
        if (!empty($management['url'])) {
            $managementStr .= " to {$management['url']}";
        }
    } else {
        $managementStr = "Disconnected";
        if (!empty($management['error'])) {
            $managementStr .= ", reason: {$management['error']}";
        }
    }

    // Signal
    $signal = $status['signal'] ?? [];
    if (!empty($signal['connected'])) {
        $signalStr = "Connected";
        if (!empty($signal['url'])) {
            $signalStr .= " to {$signal['url']}";
        }
    } else {
        $signalStr = "Disconnected";
        if (!empty($signal['error'])) {
            $signalStr .= ", reason: {$signal['error']}";
        }
    }


    // Interface
    $interfaceType = !empty($status['kernelInterface']) ? "Kernel" : "Userspace";
    $interfaceIP = $status['netbirdIp'] ?? 'N/A';
    if (empty($interfaceIP)) {
        $interfaceType = "N/A";
        $interfaceIP = "N/A";
    }

    // Relays
    $relaysStr = '';
    if (!empty($status['relays']['details'])) {
        foreach ($status['relays']['details'] as $relay) {
            $available = !empty($relay['available']) ? "Available" : "Unavailable";
            $reason = !empty($relay['error']) ? ", reason: {$relay['error']}" : "";
            $relaysStr .= "\n  [{$relay['uri']}] is {$available}{$reason}";
        }
    } else {
        $relaysStr = sprintf("%d/%d Available", $status['relays']['available'] ?? 0, $status['relays']['total'] ?? 0);
    }

    // DNS/Nameservers
    $dnsStr = '';
    if (!empty($status['dnsServers'])) {
        foreach ($status['dnsServers'] as $group) {
            $enabled = !empty($group['enabled']) ? "Available" : "Unavailable";
            $reason = !empty($group['error']) ? ", reason: {$group['error']}" : "";
            $domains = !empty($group['domains']) ? implode(", ", $group['domains']) : ".";
            $servers = implode(", ", $group['servers']);
            $dnsStr .= "\n  [{$servers}] for [{$domains}] is {$enabled}{$reason}";
        }
    } else {
        $enabledCount = 0;
        foreach ($status['dnsServers'] ?? [] as $group) {
            if (!empty($group['enabled'])) {
                $enabledCount++;
            }
        }
        $dnsStr = sprintf("%d/%d Available", $enabledCount, count($status['dnsServers'] ?? []));
    }

    // Quantum resistance
    $quantum = "false";
    if (!empty($status['rosenpassEnabled'])) {
        $quantum = "true" . (!empty($status['rosenpassPermissive']) ? " (permissive)" : "");
    }

    // Lazy connection
    $lazy = !empty($status['lazyConnectionEnabled']) ? "true" : "false";

    // Networks
    $networks = !empty($status['networks']) ? implode(", ", $status['networks']) : "-";

    // Peers
    $peersCount = sprintf("%d/%d Connected", $status['peers']['connected'] ?? 0, $status['peers']['total'] ?? 0);


    $lines = [];
    $lines[] = "Daemon version: {$status['daemonVersion']}";
    $lines[] = "CLI version: {$status['cliVersion']}";
    $lines[] = "Management: {$managementStr}";
    $lines[] = "Signal: {$signalStr}";
    $lines[] = "Relays: {$relaysStr}";
    $lines[] = "Nameservers: {$dnsStr}";
    $lines[] = "FQDN: {$status['fqdn']}";
    $lines[] = "NetBird IP: {$interfaceIP}";
    $lines[] = "Interface type: {$interfaceType}";
    $lines[] = "Quantum resistance: {$quantum}";
    $lines[] = "Lazy connection: {$lazy}";
    $lines[] = "Networks: {$networks}";
    $lines[] = "Forwarding rules: {$status['forwardingRules']}";
    $lines[] = "Peers count: {$peersCount}";
    return implode("\n", $lines);
}

function netbird_get_peers_detail(array $peers, bool $rosenpassEnabled, bool $rosenpassPermissive): string {
    $peersString = '';

    $details = !empty($peers['details']) ? $peers['details'] : [];
    foreach ($details as $peerState) {
        $localICE = !empty($peerState['iceCandidateType']['local']) ? $peerState['iceCandidateType']['local'] : '-';
        $remoteICE = !empty($peerState['iceCandidateType']['remote']) ? $peerState['iceCandidateType']['remote'] : '-';

        $localICEEndpoint = !empty($peerState['iceCandidateType']['local']) ? $peerState['iceCandidateType']['local'] : '-';
        $remoteICEEndpoint = !empty($peerState['iceCandidateType']['remote']) ? $peerState['iceCandidateType']['remote'] : '-';

        $rosenpassEnabledStatus = 'false';
        if ($rosenpassEnabled) {
            if (!empty($peerState['quantumResistance'])) {
                $rosenpassEnabledStatus = 'true';
            } else {
                if ($rosenpassPermissive) {
                    $rosenpassEnabledStatus = "false (remote didn't enable quantum resistance)";
                } else {
                    $rosenpassEnabledStatus = "false (connection won't work without a permissive mode)";
                }
            }
        } else {
            if (!empty($peerState['quantumResistance'])) {
                $rosenpassEnabledStatus = "false (connection might not work without a remote permissive mode)";
            }
        }

        $networks = '-';
        if (!empty($peerState['networks']) && is_array($peerState['networks'])) {
            sort($peerState['networks']);
            $networks = implode(', ', $peerState['networks']);
        }

        // Convert timestamps to DateTime objects for timeAgo
        $lastStatusUpdate = (!empty($peerState['lastStatusUpdate'])) ? new DateTimeImmutable($peerState['lastStatusUpdate']) : new DateTimeImmutable('@0');
        $lastWireguardHandshake = (!empty($peerState['lastStatusUpdate'])) ? new DateTimeImmutable($peerState['lastStatusUpdate']) : new DateTimeImmutable('@0');

        $latency = isset($peerState['latency']) ? floatval($peerState['latency']) / 1_000_000 . ' ms' : '-';

        $peerString = sprintf(
            "\n %s:\n".
            "  NetBird IP: %s\n".
            "  Public key: %s\n".
            "  Status: %s\n".
            "  -- detail --\n".
            "  Connection type: %s\n".
            "  ICE candidate (Local/Remote): %s/%s\n".
            "  ICE candidate endpoints (Local/Remote): %s/%s\n".
            "  Relay server address: %s\n".
            "  Last connection update: %s\n".
            "  Last WireGuard handshake: %s\n".
            "  Transfer status (received/sent) %s/%s\n".
            "  Quantum resistance: %s\n".
            "  Networks: %s\n".
            "  Latency: %s\n",
            // FQDN safe string fallback
            $peerState['fqdn'],
            $peerState['netbirdIp'],
            $peerState['publicKey'] ,
            $peerState['status'],
            $peerState['connectionType'] ?? '-',
            $localICE,
            $remoteICE,
            $localICEEndpoint,
            $remoteICEEndpoint,
            $peerState['relayAddress'] ?? '-',
            netbird_get_elapsed_time($lastStatusUpdate),
            netbird_get_elapsed_time($lastWireguardHandshake),
            netbird_format_bytes((int)($peerState['transferReceived'] ?? 0)),
            netbird_format_bytes((int)($peerState['transferSent'] ?? 0)),
            $rosenpassEnabledStatus,
            $networks,
            $latency
        );

        $peersString .= $peerString;
    }

    return $peersString;
}

function netbird_get_elapsed_time(DateTimeInterface $dt): string {
    if ($dt->getTimestamp() === 0) {
        return '-';
    }

    $units = ['y' => 'year', 'm' => 'month', 'd' => 'day', 'h' => 'hour', 'i' => 'minute', 's' => 'second'];
    $diff = (new DateTimeImmutable())->diff($dt);

    foreach ($units as $key => $label) {
        $value = $diff->$key;
        if ($value > 0) {
            return "$value $label" . ($value > 1 ? 's' : '') . " ago";
        }
    }

    return 'Now';
}

function netbird_format_bytes(int $bytes): string {
    $unit = 1024;

    if ($bytes < $unit) {
        return $bytes . ' B';
    }

    $units = ['Ki', 'Mi', 'Gi', 'Ti', 'Pi', 'Ei'];
    $exp = (int) (log($bytes) / log($unit));
    $prefix = $units[$exp - 1];
    $value = $bytes / ($unit ** $exp);

    return sprintf('%.1f %sB', $value, $prefix);
}


?>