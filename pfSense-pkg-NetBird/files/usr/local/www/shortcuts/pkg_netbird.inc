<?php
/*
 * pkg_netbird.xml
 *
 * part of pfSense (https://www.pfsense.org)
 * Copyright (c) 2022-2025 Rubicon Communications, LLC (Netgate)
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


global $shortcuts;


$shortcuts['netbird'] = [];
$shortcuts['netbird']['main'] = '/pkg_edit.php?xml=netbird.xml';
$shortcuts['netbird']['auth'] = '/netbird_auth.php';
$shortcuts['netbird']['status'] = '/netbird_status.php';
$shortcuts['netbird']['service'] = 'netbird';
