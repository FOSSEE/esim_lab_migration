<?php
function generate_lm_pdf()
	{
		$mpath = drupal_get_path('module', 'lab_migration');
		//var_dump($mpath);die;
		require($mpath . '/pdf/fpdf/fpdf.php');
		require($mpath . '/pdf/phpqrcode/qrlib.php');
		global $user;
		$x = $user->uid;
		$proposal_id = arg(3); // proposal id of lab
		$lab_certi_id = arg(4);
		if ($proposal_id != NULL && $lab_certi_id != NULL)
			{
				$query3 = db_query("SELECT * FROM lab_migration_certificate WHERE proposal_id= :prop_id AND id=:certi_id", array(
						':prop_id' => $proposal_id,
						":certi_id" => $lab_certi_id
				));
				$data3 = $query3->fetchObject();
				$pdf = new FPDF('L', 'mm', 'Letter');
						if (!$pdf)
							{
								echo "Error!";
							} //!$pdf
						$pdf->AddPage();
						$image_bg = $mpath . "/pdf/images/bg_cert.png";
						$pdf->Image($image_bg, 0, 0, $pdf->GetPageWidth(), $pdf->GetPageHeight());
						//$pdf->Rect(5, 5, 267, 207, 'D'); //For A4
						$pdf->SetMargins(18, 1, 18);
						//$pdf->Line(7.0, 7.0, 270.0, 7.0);
						//$pdf->Line(7.0, 7.0, 7.0, 210.0);
						//$pdf->Line(270.0, 210.0, 270.0, 7.0);
						//$pdf->Line(7.0, 210.0, 270.0, 210.0);
						$path = drupal_get_path('module', 'lab_migration');
						//$image1 = $mpath . "/pdf/images/scilab.png";
						$pdf->Ln(30);
						//$pdf->Cell(200, 8, $pdf->Image($image1, 100, 19, 0, 28), 0, 1, 'C');
						/*$pdf->SetFont('Times', 'BI', 25);
						$pdf->SetTextColor(139, 69, 19);
						$pdf->Cell(240, 8, 'Certificate of Participation', '0', 1, 'C');
						$pdf->Ln(3);*/
						$pdf->SetFont('Times', 'BI', 14);
						$pdf->SetTextColor(0, 0, 0);
						$pdf->Cell(320, 8, 'This is to certify that', '0', '1', 'C');
						$pdf->Ln(0);
						$pdf->SetFont('Times', 'I', 16);
						$pdf->SetTextColor(139, 69, 19);
						$pdf->Cell(320, 12, $data3->name_title . ' ' . $data3->name, '0', 1, 'C');
						$pdf->Ln(0);
						$pdf->SetFont('Times', 'I', 14);
						$pdf->SetTextColor(0, 0, 0);
						$pdf->Cell(320, 12, 'from the Department of ' . $data3->department . ', ', '0', '1', 'C');
						$pdf->Ln(0);
						$pdf->Cell(320, 8, $data3->institute_name . ', ' . $data3->institute_address, '0', '1', 'C');
						$pdf->Ln(0);
						$pdf->Cell(320, 8, ' has developed the solutions in eSim for the ', '0', '1', 'C');
						$pdf->Ln(0);
						$pdf->Cell(320, 12, $data3->lab_name . ' (' . $data3->semester_details . ').', '0', '1', 'C');
						$pdf->Cell(320, 5, '', '0', '1', 'C');
						$pdf->SetX(125);
						$pdf->SetTextColor(0, 0, 0);
						$pdf->write(0, 'The work done is available at ');
						$pdf->SetFont('Times', 'U' , '14');
						$pdf->SetTextColor(139, 69, 19);
						$pdf->write(0, 'https://esim.fossee.in/', 'https://esim.fossee.in/');
						$pdf->SetFont('', '');
						$pdf->SetTextColor(0, 0, 0);
						$pdf->write(0, '.', '.');
						//$proposal_get_id=0;
						$UniqueString = "";
						$tempDir = $path . "/pdf/temp_prcode/";
						//$UniqueString=generateRandomString();
						$query = db_select('lab_migration_certificate_qr_code');
						$query->fields('lab_migration_certificate_qr_code', array(
								'qr_code',
								'certificate_id'
						));
						//$query->condition('qr_code', $UniqueString);
						$query->condition('proposal_id', $proposal_id);
						$query->condition('certificate_id', $lab_certi_id);
						$result = $query->execute();
						$data = $result->fetchObject();
						$DBString = $data->qr_code;
						//$proposal_get_id=$data->proposal_id;
						if ($DBString == "" || $DBString == "null")
							{
								$UniqueString = generateRandomString();
								$query = "
								INSERT INTO lab_migration_certificate_qr_code
								(proposal_id,qr_code,certificate_id,gen_date)
								VALUES
								(:proposal_id,:qr_code,:certificate_id,:gen_date)
								";
								$args = array(
										":proposal_id" => $proposal_id, // id is not the proposal id its lab migration certificate table id
										":qr_code" => $UniqueString,
										":certificate_id" => $lab_certi_id,
										":gen_date" => time()
								);
								/* storing the row id in $result */
								$result = db_query($query, $args, array(
										'return' => Database::RETURN_INSERT_ID
								));
							} //$DBString == "" || $DBString == "null"
						else
							{
								$UniqueString = $DBString;
							}
						$codeContents = "https://esim.fossee.in/lab-migration/certificates/verify/" . $UniqueString;
						$fileName = 'generated_qrcode.png';
						$pngAbsoluteFilePath = $tempDir . $fileName;
						$urlRelativeFilePath = $path . "/pdf/temp_prcode/" . $fileName;
						QRcode::png($codeContents, $pngAbsoluteFilePath);
						$pdf->SetY(85);
						$pdf->SetX(320);
						$pdf->Ln(10);
						$sign1 = $path . "/pdf/images/sign1.png";
						//$sign2 = $path . "/pdf/images/sign2.png";
						$pdf->Image($sign1, $pdf->GetX()+110, $pdf->GetY() + 40, 85, 0);
						//$pdf->Image($sign2, $pdf->GetX()+160, $pdf->GetY() + 45, 85, 0);
						$pdf->Image($pngAbsoluteFilePath, $pdf->GetX()+15, $pdf->GetY()+70, 30, 0);
						$fossee = $path . "/pdf/images/fossee.png";
						$moe = $path . "/pdf/images/moe.png"; 
						$pdf->Image($fossee, $pdf->GetX()+80, $pdf->GetY()+80, 50, 0);
						$pdf->Image($moe, $pdf->GetX()+180, $pdf->GetY()+80, 40, 0);
						//$pdf->SetX(29);
						//$pdf->SetY(-50);
						$pdf->Ln(2);
						$ftr_line = $path . "/pdf/images/bottom_line.png"; 
						$pdf->Image($ftr_line, $pdf->GetX()+50, $pdf->GetY()+102, 150, 0);
						$pdf->SetFont('Times', 'I', 14);
						$pdf->SetLeftMargin(40);
						$pdf->GetY()+60;
						$pdf->Ln(62);
						$pdf->Cell(320, 8, $UniqueString, '0', '1', 'L');
						//$pdf->Ln(6);
						$pdf->SetFont('Arial', 'I', 14);
						$pdf->SetTextColor(0, 0, 0);
						$filename = str_replace(' ', '-', $data3->name) . '-eSim-Lab-migration-Participation-Certificate.pdf';
						$file     = $path . '/pdf/temp_certificate/' . $proposal_id . '_' . $filename;
						$pdf->Output($file, 'F');
						ob_clean();
						header("Pragma: public");
						header("Expires: 0");
						header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
						header("Cache-Control: public");
						header("Content-Description: File Transfer");
						header("Content-Type: application/pdf");
						header("Content-Disposition: attachment; filename=" . $filename);
						header("Content-Length: " . filesize($file));
						header("Content-Transfer-Encoding: binary");
						header("Expires: 0");
						header("Pragma: no-cache");
						flush();
						$fp = fopen($file, "r");
						while (!feof($fp)) {
							echo fread($fp, filesize($file));
							flush();
						} //!feof($fp)
						ob_end_flush();
						ob_clean();
						fclose($fp);
						unlink($file);
						//drupal_goto('flowsheeting-project/certificate');
						return;
					 //$data3->type = 'Proposer'
				}
		else
			{
				drupal_set_message('Your lab Is Still Under Review.', 'status');
			}
	}
function generateRandomString($length = 5)
	{
		$characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
		$charactersLength = strlen($characters);
		$randomString = '';
		for ($i = 0; $i < $length; $i++)
			{
				$randomString .= $characters[rand(0, $charactersLength - 1)];
			} //$i = 0; $i < $length; $i++
		return $randomString;
	}


