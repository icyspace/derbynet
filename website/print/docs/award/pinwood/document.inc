<?php

require_once('inc/photo-config.inc');
require_once('inc/path-info.inc');

class PinewoodCertificate extends PrintableAwardDocument {
  function name() { return "Pinewood Certificate"; }
  function get_available_options() {
    return array(
      'signatory1' => array('type' => 'string',
                             'desc' => 'First Signatory',
                            'default' => 'Cubmaster')
        );
  }

  function StartDocument() {
    $this->initialize_pdf('L', 'in', 'Letter');
    $this->initialize_layout(11.0, 8.5);
  }

  protected function DrawUnderlay($x, $y, $w, $h) {
    $this->pdf->SetAlpha(0.75);
    $this->pdf->SetFillColor(255, 255, 255);
    $this->pdf->RoundedRect($x, $y, $w, $h, 0.125, 'F');
    $this->pdf->SetAlpha(1);
  }

  // Horizontally-centered underlay
  protected function DrawCenteredUnderlay($cx, $y, $w, $h) {
    $this->DrawUnderlay($cx - $w / 2, $y, $w, $h);
  }
  
  // $award argument contains these keys:
  //
  // awardid
  // awardname
  // awardtype
  // classid
  // rankid
  // racerid
  // carnumber, firstname, lastname, carname, class, rank, imagefile, carphoto
  //
  function DrawOneAt($left, $top, &$award) {
    $params = $this->layout->GetParameters()['doc'];
    $cx = $left + $params['width']/2;
    $cy = $top + $params['height']/2;

    $this->pdf->AddFont('BlackChancery', '', 'black-chancery.php');
    $this->pdf->AddFont('helvetica', '', 'helvetica.php');
    $this->pdf->AddFont('hk-grotesk.semibold', '', 'hk-grotesk.semibold.php');

    $this->pdf->Image(dirname(__FILE__).'/PinewoodCertificate.png',
                      $left, $top, $params['width']);

    $this->pdf->SetFont('hk-grotesk.semibold', '', 30);

    // Award name
    $award_name_width = $this->pdf->GetStringWidth($award['awardname']);
    //$this->DrawCenteredUnderlay($cx, $top + 2.0, $award_name_width + 1.0, 0.75);
    $this->pdf->CenteredSequence($cx + 1.1 , $top + 3.5, array($award['awardname']));

    $this->pdf->SetLineWidth(0.01);
    // Recipient
    if (isset($award['firstname'])) {
      
      $recip = strtoupper($award['firstname'].' '.$award['lastname']);
      $recip_width = $this->pdf->GetStringWidth($recip);
      $recip_width += 1.0;
     // $recip = $recip. $recip_width;
      if ($recip_width > 7.3) {
        $this->pdf->SetFont('hk-grotesk.semibold', '', 34);
      } else {
        $this->pdf->SetFont('hk-grotesk.semibold', '', 40);
      }

     // $this->DrawCenteredUnderlay($cx, $cy - 0.5, $recip_width, 1.0);
      $this->pdf->CenteredSequence($cx + 1, $cy + .9 , array($recip));
    } else {
      $this->pdf->SetDrawColor(0);
      $recip_width = 6.0;
    //  $this->DrawCenteredUnderlay($cx, $cy - 0.75, $recip_width + 1.0, 1.0);
      $this->pdf->Line($cx - $recip_width / 2, $cy, $cx + $recip_width / 2, $cy);
    }

    $this->pdf->SetFont('Times', '', 15);
    $this->pdf->SetLineWidth(0.01);
    $this->pdf->SetDrawColor(0);

    // Signature lines:
   // $sig_width = 3.0;   // Length of each signature line
   // $sig_margin = 0.85;  // Margin from signature line to the nearest horizontal edge

  }
}

?>
